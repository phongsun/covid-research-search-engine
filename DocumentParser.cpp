//
// Created by Peter Sun on 11/14/20.
//


#include "DocumentParser.h"

DocumentParser::DocumentParser(const string &corpusPath, const string &stopwordPath, const unordered_map<string, ArticleMetaData> &metaDataMap){
    this->corpusPath = corpusPath;
    this->stopwordPath = stopwordPath;
    this->metaDataMap = metaDataMap;
}

void DocumentParser::parse(DSAvlTree<IndexNodeData>* keywordIndex, DSHashTable<string, unordered_set<string>> *authorIndex){
    int numberOfKeyWordsForArcticles = 0;

    this->totalFilesLoaded = 0;
    // 1. load full-text document ids and publishing dates from metadata CSV file
    // The code only parse a file if the document id is in the metadata file

    // 1.1. temporary storage to store words and the corresponding stemmed words
    // in order to avoid stem a word that was stemmed before
    unordered_map<string, string> stemmedWordMap;
    unordered_map<string, unsigned int> stemmedWordCount;
    unordered_map<string, unsigned int> orginalWordCount;
    unordered_map<string, unsigned int> uniqueAuthorCount;
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
                this->totalFilesLoaded++;

                // 10. create a articleData object to represent current document
                ArticleData articleData = ArticleData(documentID);
                string_view lastNameView;
                string lastName;
                string_view firstNameView;
                string firstName;
                // 11. parse author's last name from json and add them to authors list in articleData object
                for (auto author : paper["metadata"]["authors"]) {
                    if (author["last"].get_string_length() > 0) {
                        lastNameView = author["last"].get_string();
                        if (author["first"].get_string_length() > 0) {
                            firstNameView = author["first"].get_string();
                        } else {
                            firstNameView = lastNameView;
                        }
                    }
                        // else if the last name is empty then use the first name
                    else {
                        lastNameView = author["first"].get_string();
                        firstNameView = lastNameView;
                    }
                    lastName = {lastNameView.begin(), lastNameView.end()};
                    firstName = {firstNameView.begin(), firstNameView.end()};
                    transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
                    transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
                    articleData.authorLastNames.insert(lastName);

                    // building unique user name stats
                    string fullName = lastName+", "+firstName;
                    uniqueAuthorCount[fullName]++;
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
                            orginalWordCount[token]++;

                            // 19. stem the non stop word
                            if (stemmedWordMap.count(token) >
                                0) { // if the token has already been stemmed before, then the stemmed word for the token is obtained and the frequency is increased
                                ++articleData.keyWordList[stemmedWordMap[token]]; // 20. increment count of the stemmed word

                                ++stemmedWordCount[stemmedWordMap[token]];
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

                                ++stemmedWordCount[token];
                            }

                        }
                    }
                } // end of parsing body text
                // calculate the sum of number of keywords for each article
                numberOfKeyWordsForArcticles += articleData.keyWordList.size();
                // 21. all the data are parsed and collected in the articleData
                // add it to the AvlTree - keywordIndex and DSHashtable - authorIndex
                this->addArticleToKeywordIndex(keywordIndex, articleData);
                this->addAuthorsToHashTable(authorIndex, articleData);
            } else { // no documentID is not full-text
                ;
            }
        } // end of creating articleData for the current article file

        if(this->maxFilesToLoad > 0 && this->totalFilesLoaded == this->maxFilesToLoad){ // control number of files to load for testing
            break;
        }
    }

    // calculate stats
    this->avgKeyWordsIndexedPerArticle = numberOfKeyWordsForArcticles / this->totalFilesLoaded;
    this->totalUniqueAuthors = uniqueAuthorCount.size();
    /**************************************
    * calculate 50 highestoriginal word counts
    **************************************/
    auto cmp = [](std::pair<int,string> const & a, std::pair<int,string> const & b)
    {
        return a.first != b.first?  a.first > b.first : a.second.compare(b.second) < 0;
    };

    for (auto wordRank : orginalWordCount) {
        this->top50OriginalWords.push_back(pair<int, string>(wordRank.second, wordRank.first));
    }
    std::sort(this->top50OriginalWords.begin(), this->top50OriginalWords.end(), cmp);
    this->top50OriginalWords.erase(this->top50OriginalWords.begin() + 50, this->top50OriginalWords.end());

    /**************************************
     * calculate 50 highest stemmed word counts
     **************************************/
    for (auto wordRank : stemmedWordCount) {

        this->top50StemmedWords.push_back(pair<int, string>(wordRank.second, wordRank.first));
    }
    std::sort(this->top50StemmedWords.begin(), this->top50StemmedWords.end(), cmp);
    this->top50StemmedWords.erase(this->top50StemmedWords.begin() + 50, this->top50StemmedWords.end());
    // Add orginal words to each stemmed word
    for (auto &topWord : this->top50StemmedWords) {
        string key = topWord.second + " (";
        auto check_value = [&](std::pair<const string, string> const &p) -> bool {
            return (p.second.compare(topWord.second) == 0);
        };

        auto end = std::end(stemmedWordMap);
        auto it = find_if(std::begin(stemmedWordMap), end, check_value);
        while (it != end) {
            key += it->first;
            key += " ";
            it = find_if(std::next(it), end, check_value);
        }
        key += ") ";
        topWord.second = key;
    }

    closedir(dir);
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

inline void DocumentParser::addArticleToKeywordIndex(DSAvlTree<IndexNodeData> *avlTree, const ArticleData &articleData){
    unsigned int noOfWordsInArticle = articleData.keyWordList.size();
    // 22. loop through all the elements in the articleData.keyWordList
    for(auto keyWordAndFreq: articleData.keyWordList){
        // 23. calculate TF for each word
        // multiplied by 100000 to make the termFreq an accurate int
        unsigned int termFreq = (keyWordAndFreq.second * 10000) / noOfWordsInArticle;

        // 24. search if the key word already in the AvlTree
        IndexNodeData keyWordNodeData;
        keyWordNodeData.keyWord = keyWordAndFreq.first;
        DSAvlNode<IndexNodeData> *node = avlTree->search(keyWordNodeData);

        // 25. if not, create a IndexNodeData object with inverted TF by document id
        // insert the IndexNodeData into the AvlTree
        if(node == nullptr){ // keyword doesn't exist in the avlTree
            keyWordNodeData.invertedTermFreq[articleData.documentID] = termFreq;
            // insert the new data because the keyword wasn't in the tree yet
            avlTree->insert(keyWordNodeData);
        }else{
            // 26. Keyword already exist in the tree, add TF for the document id to the node for the keyword.
            node->element.invertedTermFreq[articleData.documentID] = termFreq;
        }
    }
}

inline void DocumentParser::addAuthorsToHashTable(DSHashTable<string, unordered_set<string>> *authorIndex, const ArticleData &input) {
    pair<string, unordered_set<string>> invertedDocumentIdsByAuthor;
    for (auto author: input.authorLastNames) {
        unordered_set<string> documentIdList;
        DSHashTable<string, unordered_set<string>>::Iterator iter = authorIndex->find(author);
        if (iter == authorIndex->end()) { // author is not in hashtable, create the index entry
            documentIdList.insert(input.documentID);
            // create a new hashtable entry
            invertedDocumentIdsByAuthor = pair<string, unordered_set<string>>(author, documentIdList);

        } else { // author is in hashtable, get index entry from hashtable
            invertedDocumentIdsByAuthor = *iter;
            // add title of this paper to the hashtable entry
            invertedDocumentIdsByAuthor.second.insert(input.documentID);
            authorIndex->erase(
                    author); // erase the current entry so we can insert a new entry for the same key
        }
        authorIndex->insert(invertedDocumentIdsByAuthor);
    }
}