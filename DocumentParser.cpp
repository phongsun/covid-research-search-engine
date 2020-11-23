//
// Created by Peter Sun on 11/14/20.
//


#include "DocumentParser.h"

DocumentParser::DocumentParser(const string &corpusPath, const string &stopwordPath, const string &metaDataFileName){
    this->corpusPath = corpusPath;
    this->stopwordPath = stopwordPath;
    this->metaDataFileName = metaDataFileName;
}

void DocumentParser::parse(DSAvlTree<IndexNodeData> &keywordIndex){
    int i = 0;
    loadMetaData();

    unordered_map<string, string> stemmedWordMap;
    unordered_set<string> stopWords = this->loadStopWords(this->stopwordPath);

    struct dirent *entry;
    DIR *dir = opendir(this->corpusPath.c_str());
    if (dir == NULL) {
        cout << "Directory not found for " << this->corpusPath << endl;
        return;
    }

    simdjson::dom::parser parser;
    while ((entry = readdir(dir)) != NULL){
        string fullPath = this->corpusPath + "/" + entry->d_name;
        // .json is only one type of file in the folder out of various types
        if (fullPath.find(".json") != string::npos) {
            i++;
            simdjson::dom::element paper = parser.load(fullPath);

            string_view documentID_v = paper["paper_id"].get_string();
            string documentID = {documentID_v.begin(), documentID_v.end()};
            // if the document id is not full text, then next
            if (this->metaDataMap.count(documentID) > 0) {
                string_view title = paper["metadata"]["title"].get_string();

                ArticleData articleData = ArticleData(documentID);
                string_view lastNameView;
                string lastName;
                // parse author's last name
                for (auto author : paper["metadata"]["authors"]) {
                    if (author["last"].get_string_length() > 0) {
                        lastNameView = author["last"].get_string();
                    }
                        // else if the last name is empty then use the first name
                    else {
                        lastNameView = author["first"].get_string();
                    }
                    lastName = {lastNameView.begin(), lastNameView.end()};
                    transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
                    articleData.authorLastNames.insert(lastName);
                }

                // parse the body text
                for (auto bodyText: paper["body_text"]) {
                    // get_string() returns a string view object
                    string_view txt_vw = bodyText["text"].get_string();
                    string txt = {txt_vw.begin(), txt_vw.end()};
                    // erase characters that aren't alpha, numeric, or a space
                    txt.erase(std::remove_if(txt.begin(), txt.end(),
                                             [](auto const &c) -> bool {
                                                 return !std::isalpha(c) && !std::isspace(c);
                                             }),
                              txt.end());
                    // convert all characters to lower case
                    transform(txt.begin(), txt.end(), txt.begin(), ::tolower);
                    istringstream iss(txt);
                    // parse words separated by space into a vector of strings
                    vector<string> tokens{istream_iterator<string>{iss},
                                          istream_iterator<string>{}};
                    for (auto token: tokens) {
                        // check stop word, check if length is greater than 2 or not
                        if (token.length() > 2 && stopWords.count(token) == 0) {
                            if (stemmedWordMap.count(token) >
                                0) { // if the token has already been stemmed before, then the stemmed word for the token is obtained and the frequency is increased
                                ++articleData.keyWordList[stemmedWordMap[token]];
                            } else { // if the token has not been stemmed before
                                //
                                // trim and stem the word
                                //Porter2Stemmer::trim(token);
                                string oldToken = token;
                                Porter2Stemmer::stem(token);
                                // count the frequency of each word that aren't stop words and stemmed
                                ++articleData.keyWordList[token];
                                // add the stemmed token to the stemmed word map
                                stemmedWordMap[oldToken] = token;
                            }
                        }
                    }
                } // end of parsing body text

                this->addArticleToKeywordIndex(keywordIndex, articleData);

            } else { // no documentID is not full-text
                ;
            }
        } // end of creating articleData for the current article file

        /*if(i > 1000){ // only three files are processed for now
            return;
        }*/
    }
    //cout << "number of files parsed: " << i << endl;
    closedir(dir);

}

inline void DocumentParser::loadMetaData(){
    string filePath;

    filePath = this->corpusPath
            + (this->corpusPath.c_str()[this->corpusPath.size() - 1] == '/' ? "" : "/" )
            + this->metaDataFileName;
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {

            // tokenize the line by comma
            // only take the full text articles (16)
            // map has key of documentId (1) and value of publish_time (9)
            std::vector<std::string> result;
            std::stringstream lineStream(line);
            std::string cell;

            // read full line
            const char *mystart = line.c_str();    // prepare to parse the line - start is position of begin of field
            bool instring{false};
            for (const char *p = mystart; *p; p++) {  // iterate through the string
                if (*p == '"')                        // toggle flag if we're btw double quote
                    instring = !instring;
                else if (*p == ',' && !instring) {    // if comma OUTSIDE double quote
                    result.push_back(string(mystart, p - mystart));  // keep the field
                    mystart = p + 1;                    // and start parsing next one
                }
            }
            result.push_back(string(mystart));   // last field delimited by end of line instead of comma
            this->metaDataMap[result[1]] = result[9];
        }
        file.close();

    } else {
        cout<< "Failed to load meta data file. Cannot find " << filePath << endl;
    }

}

inline unordered_set<string> DocumentParser::loadStopWords(const string &filePath){
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
    unsigned int noOfWordsInArticle = articleData.keyWordList.size();
    // loop through all the keywords in the article
    for(auto keyWordAndFreq: articleData.keyWordList){
        // multiplied by 100000 to make the termFreq an accurate int
        unsigned int termFreq = (keyWordAndFreq.second * 100000) / noOfWordsInArticle;
        IndexNodeData keyWordNodeData;
        keyWordNodeData.keyWord = keyWordAndFreq.first;
        DSAvlNode<IndexNodeData> *node = avlTree.search(keyWordNodeData);
        if(node == nullptr){ // keyword doesn't exist in the avlTree
            keyWordNodeData.invertedTermFreq[articleData.documentID] = termFreq;
            // insert the new data because the keyword wasn't in the tree yet
            avlTree.insert(keyWordNodeData);
        }else{ // add the word frequency by the documentID of the article to the node; no need to insert because the node is already in the tree
            node->element.invertedTermFreq[articleData.documentID] = termFreq;
        }
    }
    // insert the keywords into the avlTree
}