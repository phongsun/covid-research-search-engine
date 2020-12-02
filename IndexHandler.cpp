//
// Created by Peter Sun on 11/25/20.
//

#include "IndexHandler.h"

IndexHandler::IndexHandler(const string &corpusPath){
    this->corpusPath = corpusPath;
    this->metaDataMap = loadMetaData(corpusPath);
    this->keyWordIndex = nullptr;
    this->authorIndex = nullptr;
}

bool IndexHandler::isIndexEmpty(){
    return this->keyWordIndex == nullptr;
}

void IndexHandler::clearIndex() {
    if (this->keyWordIndex != nullptr) {
        delete this->keyWordIndex;
        this->keyWordIndex = nullptr;
    }
    if (this->authorIndex != nullptr) {
        delete this->authorIndex;
        this->authorIndex = nullptr;
    }
    // delete the persistent indices

    this->totalArticlesIndexed = 0;
    this->avgWordsIndexedPerArticle = 0;
    this->topStemmed50Words.clear();
    this->top50OriginalWords.clear();
    this->topWords.clear();
    this->totalWordsIndexed =  0;
    this->totalUniqueAuthors = 0;
}

bool IndexHandler::createIndex(){
    this->clearIndex();

    DocumentParser documentParser = DocumentParser(this->corpusPath, this->stopWordFile, this->metaDataMap);
    documentParser.maxFilesToLoad = this->maxFilesToLoad;

    this->keyWordIndex = new DSAvlTree<IndexNodeData>();
    this->authorIndex = new DSHashTable<string, unordered_set<string>>();
    documentParser.parse(this->keyWordIndex, this->authorIndex);

    this->totalArticlesIndexed = documentParser.totalFilesLoaded;
    this->avgWordsIndexedPerArticle = documentParser.avgKeyWordsIndexedPerArticle;
    this->topStemmed50Words = documentParser.top50StemmedWords;
    this->top50OriginalWords = documentParser.top50OriginalWords;
    for (int i = 0; i < 50; i++) {
        this->topWords.push_back(documentParser.top50OriginalWords[i].second);
    }
    this->totalUniqueAuthors = documentParser.totalUniqueAuthors;
    this->totalWordsIndexed =  this->keyWordIndex->count();

    return this->totalWordsIndexed > 0;
}

IndexNodeData* IndexHandler::searchByKeyword(const string &keyWord){
    if (this->keyWordIndex == nullptr) {
        cout << "The Keyword Index is empty. ";
        return nullptr;
    }

    IndexNodeData searchQuery;
    searchQuery.keyWord = keyWord;
    DSAvlNode<IndexNodeData> *node = this->keyWordIndex->search(searchQuery);
    if(node == nullptr){ // no result found
        return nullptr;
    }else {
        node->element.calculateIdf(this->totalArticlesIndexed);
        return &node->element;
    }
}

unordered_set<string> IndexHandler::searchByAuthor(const string &author){
    unordered_set<string> documentIdList;
    if (this->authorIndex == nullptr) {
        cout << "The Author Index is empty. ";
        return documentIdList;
    }

    pair<string, unordered_set<string>> invertedDocumentIdsByAuthor;
    DSHashTable<string, unordered_set<string>>::Iterator iter = this->authorIndex->find(author);
    if (iter == authorIndex->end()) { // author is not in hashtable, create the index entry
        return documentIdList;

    } else { // author is in hashtable, get index entry from hashtable
        invertedDocumentIdsByAuthor = *iter;
        documentIdList = invertedDocumentIdsByAuthor.second;
        return documentIdList;
    }
}

bool IndexHandler::persistIndices(){

    // if all are restored successfully, return true
    if (this->persistKeywordIndex() && this->persistAuthorIndex() && this->persistStats()) {
        return true;
    } else {
        return false;
    }
}

bool IndexHandler::restoreIndices(const string &dir) {
    if(dir.length() > 0){
        this->setPersistentDir(dir);
    }
    // clear the index
    this->clearIndex();

    // if all are restored successfully, return true
    if (this->restoreKeywordIndex() && this->restoreAuthorIndex() && this->restoreStats()) {
        return true;
    } else {
        // if failed to restore, clear the index again
        this->clearIndex();
        return false;
    }
}


bool IndexHandler::persistStats(){
    string json = "{\"stats\": {\"tw\":[";
    for (auto w : this->topWords) {
        json += "\"" + w + "\",";
    }
    json[json.length() - 1] = ']';

    json += ",\"totalArticlesIndexed\":\"" + to_string(this->totalArticlesIndexed) + "\",";
    json += "\"avgWordsIndexedPerArticle\":\"" + to_string(this->avgWordsIndexedPerArticle) + "\",";
    json += "\"totalWordsIndexed\":\"" + to_string(this->totalWordsIndexed) + "\",";
    json += "\"totalUniqueAuthors\":\"" + to_string(this->totalUniqueAuthors) + "\"}}";

    ofstream fileToWrite (this->getStatsFilePath());
    if (fileToWrite.is_open()) {
        fileToWrite<<json;
        fileToWrite.close();
        return true;
    }

    return false;
}
bool IndexHandler::restoreStats() {
    simdjson::dom::parser jsonParser;
    // load json file
    simdjson::dom::element doc = jsonParser.load(this->getStatsFilePath());
    this->topWords.clear();
    for (auto d: doc["stats"]["tw"]) {
        string_view docIdV = d.get_string();
        string docId = {docIdV.begin(), docIdV.end()};
        this->topWords.push_back(docId);
    }

    string_view numV = doc["stats"]["totalArticlesIndexed"].get_string();
    string num = {numV.begin(), numV.end()};
    totalArticlesIndexed = atoi(num.c_str());

    numV = doc["stats"]["avgWordsIndexedPerArticle"].get_string();
    num = {numV.begin(), numV.end()};
    avgWordsIndexedPerArticle = atoi(num.c_str());

    numV = doc["stats"]["totalWordsIndexed"].get_string();
    num = {numV.begin(), numV.end()};
    totalWordsIndexed = atoi(num.c_str());

    numV = doc["stats"]["totalUniqueAuthors"].get_string();
    num = {numV.begin(), numV.end()};
    totalUniqueAuthors = atoi(num.c_str());

    return this->topWords.size() == 50;
}
bool IndexHandler::persistAuthorIndex() {
    string json = "{\"ht\": [";
    DSHashTable<string, unordered_set<string>>::Iterator iter = this->authorIndex->begin();
    while (iter != this->authorIndex->end()) {
        pair<string, unordered_set<string>> el = *iter;
        string elJson = "{\"k\":\"" + el.first;
        elJson += "\", \"v\":[";

        for (auto d : el.second) {
            elJson += "\"" + d + "\",";
        }
        elJson[elJson.length() - 1] = ']'; // replace the last comma
        elJson += "},";

        json += elJson; // add element to the bigger string
        iter++;
    }
    json[json.length() - 1] = ']';
    json += "}";


    ofstream fileToWrite (this->getAuthorIndexFilePath());
    if (fileToWrite.is_open()) {
        fileToWrite<<json;
        fileToWrite.close();
        return true;
    }

    return false;
}
bool IndexHandler::restoreAuthorIndex() {
    simdjson::dom::parser jsonParser;
    // load json file
    simdjson::dom::element doc = jsonParser.load(this->getAuthorIndexFilePath());
    this->authorIndex = new DSHashTable<string, unordered_set<string>>();
    for (auto kv: doc["ht"]) {
        // parse key
        string_view authorV = kv["k"].get_string();
        string author = {authorV.begin(), authorV.end()};

        // parse document list
        unordered_set<string> docIdList;
        for (auto d: kv["v"].get_array()) {
            string_view docIdV = d.get_string();
            string docId = {docIdV.begin(), docIdV.end()};
            docIdList.insert(docId);
        }
        this->authorIndex->insert(pair<string, unordered_set<string>>(author, docIdList));
    }

    return this->authorIndex->count() > 0;
}
bool IndexHandler::persistKeywordIndex(){
    if (this->keyWordIndex == nullptr) {
        cout << "The Keyword Index is empty. ";
        return false;
    }
    string keyWordIndexFilePath = this->getKeyWordIndexFilePath();
    ofstream fileToWrite (keyWordIndexFilePath);
    if (fileToWrite.is_open()) {
        this->keyWordIndex->serialize(fileToWrite);
        fileToWrite.close();
        return true;
    } else {
        return false;
    }
}
bool IndexHandler::restoreKeywordIndex(){
    ifstream fileToRead (this->getKeyWordIndexFilePath());
    if (fileToRead.is_open())
    {
        this->keyWordIndex = new DSAvlTree<IndexNodeData>();
        this->keyWordIndex->deserialize(fileToRead);
        fileToRead.close();
        return true;
    }
    return false;
}

unordered_map<string, ArticleMetaData> IndexHandler::loadMetaData(const string &corpusPath){
    unordered_map<string, ArticleMetaData> metaDataMap;

    string filePath;

    filePath = corpusPath
               + (corpusPath.c_str()[corpusPath.size() - 1] == '/' ? "" : "/" )
               + "metadata-cs2341.csv";
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
            metaData.datePublished = result[9];
            metaData.author = result[10];
            metaData.title = result[3];
            metaData.abstract = result[8];
            metaData.publication = result[11];

            metaDataMap[result[1]] = metaData;
        }
        file.close();

    } else {
        cout<< "Failed to load meta data file. Cannot find " << filePath << endl;
    }

    return metaDataMap;
}

