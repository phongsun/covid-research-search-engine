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
    // 1. load full-text document ids and publishing dates from metadata CSV file
    // The code only parse a file if the document id is in the metadata file
    loadMetaData();

    // 1.1. temporary storage to store words and the corresponding stemmed words
    // in order to avoid stem a word that was stemmed before
    unordered_map<string, string> stemmedWordMap;

    // 2. load stop words from stopwords.txt file
    unordered_set<string> stopWords = this->loadStopWords(this->stopwordPath);

    // 3. open the corpus directory
    struct dirent *entry;
    DIR *dir = opendir(this->corpusPath.c_str());
    if (dir == NULL) {
        cout << "Directory not found for " << this->corpusPath << endl;
        return;
    }

    // 4. use 3rd party simdjson library as the json parser
    simdjson::dom::parser parser;

    // 5. loop through files in the directory
    while ((entry = readdir(dir)) != NULL){
        string fullPath = this->corpusPath + "/" + entry->d_name; // create full path for current file
        // 6. if file is not .json, then back to the while loop
        if (fullPath.find(".json") != string::npos) {
            // 7. load file into the json element paper which represents an article
            simdjson::dom::element paper = parser.load(fullPath);
            // 8. get paper_id from json
            string_view documentID_v = paper["paper_id"].get_string();
            string documentID = {documentID_v.begin(), documentID_v.end()};
            // 9. check if document_id is in meta data, if not back to the while loop
            if (this->metaDataMap.count(documentID) > 0) {
                i++;

                // 10. create a articleData object to represent current document
                ArticleData articleData = ArticleData(documentID);
                string_view lastNameView;
                string lastName;
                // 11. parse author's last name from json and add them to authors list in articleData object
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

                // 12. loop through the body text of the article
                for (auto bodyText: paper["body_text"]) {
                    // 13. get a line of text from json
                    // get_string() returns a string view object
                    string_view txt_vw = bodyText["text"].get_string();
                    string txt = {txt_vw.begin(), txt_vw.end()};

                    // 14. erase characters that aren't alpha or space (delimiters)
                    txt.erase(std::remove_if(txt.begin(), txt.end(),
                                             [](auto const &c) -> bool {
                                                 return !std::isalpha(c) && !std::isspace(c);
                                             }),
                              txt.end());
                    // 15. convert all characters in the line to lower case
                    transform(txt.begin(), txt.end(), txt.begin(), ::tolower);
                    istringstream iss(txt);
                    // 16. parse words separated by space into a vector of strings
                    vector<string> tokens{istream_iterator<string>{iss},
                                          istream_iterator<string>{}};
                    // 17. loop through all the words in current line
                    for (auto token: tokens) {
                        // 18. check if current word is a stop word, if so go back to for loop
                        if (token.length() > 2 && stopWords.count(token) == 0) {
                            // 19. stem the non stop word
                            if (stemmedWordMap.count(token) >
                                0) { // if the token has already been stemmed before, then the stemmed word for the token is obtained and the frequency is increased
                                ++articleData.keyWordList[stemmedWordMap[token]]; // 20. increment count of the stemmed word
                            } else { // if the token has not been stemmed before
                                //
                                // trim and stem the word
                                //Porter2Stemmer::trim(token);
                                string oldToken = token;
                                Porter2Stemmer::stem(token);
                                // 20. increment count of the stemmed word
                                ++articleData.keyWordList[token];
                                // add the stemmed token to the stemmed word map
                                stemmedWordMap[oldToken] = token;
                            }
                        }
                    }
                } // end of parsing body text

                // 21. all the data are parsed and collected in the articleData
                // add it to the AvlTree - keywordIndex
                this->addArticleToKeywordIndex(keywordIndex, articleData);

            } else { // no documentID is not full-text
                ;
            }
        } // end of creating articleData for the current article file

        if(i == 1000){ // only 1000 files are processed for now
            cout << "number of files parsed: " << i << endl;
            return;
        }
    }
    cout << "number of files parsed: " << i << endl;
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
            ArticleMetaData metaData;
            metaData.publicationDate = result[9];
            metaData.title = result[3];
            metaData.abstract = result[8];


            this->metaDataMap[result[1]] = metaData;
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
    // 22. loop through all the elements in the articleData.keyWordList
    for(auto keyWordAndFreq: articleData.keyWordList){
        // 23. calculate TF for each word
        // multiplied by 100000 to make the termFreq an accurate int
        unsigned int termFreq = (keyWordAndFreq.second * 100000) / noOfWordsInArticle;

        // 24. search if the key word already in the AvlTree
        IndexNodeData keyWordNodeData;
        keyWordNodeData.keyWord = keyWordAndFreq.first;
        DSAvlNode<IndexNodeData> *node = avlTree.search(keyWordNodeData);

        // 25. if not, create a IndexNodeData object with inverted TF by document id
        // insert the IndexNodeData into the AvlTree
        if(node == nullptr){ // keyword doesn't exist in the avlTree
            keyWordNodeData.invertedTermFreq[articleData.documentID] = termFreq;
            // insert the new data because the keyword wasn't in the tree yet
            avlTree.insert(keyWordNodeData);
        }else{
            // 26. Keyword already exist in the tree, add TF for the document id to the node for the keyword.
            node->element.invertedTermFreq[articleData.documentID] = termFreq;
        }
    }

    this->finalIndex = avlTree;
}