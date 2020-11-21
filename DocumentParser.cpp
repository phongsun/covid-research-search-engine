//
// Created by Peter Sun on 11/14/20.
//


#include "DocumentParser.h"

DocumentParser::DocumentParser(const string &corpusPath, const string &stopwordPath){
    this->corpusPath = corpusPath;
    this->stopwordPath = stopwordPath;
}

void DocumentParser::parse(DSAvlTree<IndexNodeData> &keywordIndex){
    int i = 0;
    int k = 0;
    unordered_map<string, string> stemmedWordMap;
    unordered_set<string> stopWords = this->loadStopWords(this->stopwordPath);

    struct dirent *entry;
    DIR *dir = opendir(this->corpusPath.c_str());
    if (dir == NULL) {
        return;
    }

    simdjson::dom::parser parser;
    while ((entry = readdir(dir)) != NULL){
        string fullPath = this->corpusPath + "/" + entry->d_name;
        // .json is only one type of file in the folder out of various types
        if (fullPath.find(".json") != string::npos) {
            i++;
            simdjson::dom::element paper = parser.load(fullPath);
            string_view title = paper["metadata"]["title"].get_string();
            string_view documentID = paper["paper_id"].get_string();
            // if the documentID has more than 2 letters then it's a valid documentID
            if (title.size() >= 3) {
                ArticleData articleData = ArticleData({documentID.begin(), documentID.end()});
                std::map<std::string, size_t> wordFrequency;
                string_view lastNameView;
                string lastName;
                // parse author's last name
                /*for(auto author : paper["metadata"]["authors"]){
                    if (author["last"].get_string_length() > 0) {
                        lastNameView = author["last"].get_string();
                    }
                    // else if the last name is empty then use the first name
                    else {
                        lastNameView = author["first"].get_string();
                    }
                    lastName = {lastNameView.begin(), lastNameView.end()};
                    transform (lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
                    articleData.authorLastNames.insert(lastName);
                }*/

                // parse the body text
                for (auto bodyText: paper["body_text"]) {
                    // get_string() returns a string view object
                    string_view txt_vw = bodyText["text"].get_string();
                    string txt = {txt_vw.begin(), txt_vw.end()};
                    // erase characters that aren't alpha, numeric, or a space
                    txt.erase(std::remove_if(txt.begin(), txt.end(),
                            []( auto const& c ) -> bool{return !std::isalpha(c)&&!std::isspace(c); } ),
                                    txt.end());
                    // convert all characters to lower case
                    transform (txt.begin(), txt.end(), txt.begin(), ::tolower);
                    istringstream iss(txt);
                    // parse words separated by space into a vector of strings
                    vector<string> tokens{istream_iterator<string>{iss},
                                          istream_iterator<string>{}};
                    for (auto token: tokens) {
                        // check stop word, check if length is greater than 2 or not
                        if (token.length() > 2 && stopWords.count(token) == 0){
                            if(stemmedWordMap.count(token) > 0){ // if the token has already been stemmed before, then the stemmed word for the token is obtained and the frequency is increased
                                ++wordFrequency[stemmedWordMap[token]];
                            }else{ // if the token has not been stemmed before
                                //
                                // trim and stem the word
                                //Porter2Stemmer::trim(token);
                                string oldToken = token;
                                Porter2Stemmer::stem(token);
                                // count the frequency of each word that aren't stop words and stemmed
                                ++wordFrequency[token];
                                // add the stemmed token to the stemmed word map
                                stemmedWordMap[oldToken] = token;
                            }
                        }
                    }
                } // end of parsing body text

                articleData.keyWordList = wordFrequency;
                this->addArticleToKeywordIndex(keywordIndex, articleData);

            } else { // no documentID should be less than 3 characters

            }
        } // end of creating articleData for the current article file

        if(i > 100){ // only three files are processed for now
            return;
        }
    }
    cout << "number of files parsed: " << i << endl;
    closedir(dir);

}

unordered_set<string> DocumentParser::loadStopWords(const string &filePath){
    const int BUFFER_SIZE = 25;
    char buffer[BUFFER_SIZE];
    ifstream myFile;
    myFile.open(filePath);
    string line;
    unordered_set<string> stopWords;
    if (myFile.is_open()) {
        while (std::getline(myFile, line))
        {
            stopWords.insert(line);
        }
    }
    myFile.close();
    return stopWords;
}

inline void DocumentParser::addArticleToKeywordIndex(DSAvlTree<IndexNodeData> &avlTree, const ArticleData &articleData){
    // loop through all the keywords in the article
    for(auto keyWordAndFreq: articleData.keyWordList){
        IndexNodeData keyWordNodeData;
        keyWordNodeData.keyWord = keyWordAndFreq.first;
        DSAvlNode<IndexNodeData> *node = avlTree.search(keyWordNodeData);
        if(node == nullptr){ // keyword doesn't exist in the avlTree
            keyWordNodeData.invertedFreq[articleData.documentID] = keyWordAndFreq.second;
            // insert the new data because the keyword wasn't in the tree yet
            avlTree.insert(keyWordNodeData);
        }else{ // add the word frequency by the documentID of the article to the node; no need to insert because the node is already in the tree
            node->element.invertedFreq[articleData.documentID] = keyWordAndFreq.second;
        }
    }
    // insert the keywords into the avlTree
}