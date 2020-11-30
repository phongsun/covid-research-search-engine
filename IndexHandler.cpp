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
    this->topStemmed50WordsData.clear();
    this->top50OriginalWordsData.clear();
    this->totalWordsIndexed =  0;
    this->totalUniqueAuthors = 0;
}

int IndexHandler::createIndex(){
    this->clearIndex();

    DocumentParser documentParser = DocumentParser(this->corpusPath, this->stopWordFile, this->metaDataMap);
    documentParser.maxFilesToLoad = this->maxFilesToLoad;

    this->keyWordIndex = new DSAvlTree<IndexNodeData>();
    this->authorIndex = new DSHashTable<string, unordered_set<string>>();
    documentParser.parse(this->keyWordIndex, this->authorIndex);

    this->totalArticlesIndexed = documentParser.totalFilesLoaded;
    this->avgWordsIndexedPerArticle = documentParser.avgKeyWordsIndexedPerArticle;
    this->topStemmed50WordsData = documentParser.top50StemmedWords;
    this->top50OriginalWordsData = documentParser.top50OriginalWords;
    this->totalUniqueAuthors = documentParser.totalUniqueAuthors;
    this->totalWordsIndexed =  this->keyWordIndex->count();

    return this->totalWordsIndexed;
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
            metaData.publicationDate = result[9];
            metaData.author = result[10];
            metaData.title = result[3];
            metaData.abstract = result[8];

            metaDataMap[result[1]] = metaData;
        }
        file.close();

    } else {
        cout<< "Failed to load meta data file. Cannot find " << filePath << endl;
    }

    return metaDataMap;
}

