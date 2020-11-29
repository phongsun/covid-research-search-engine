//
// Created by Peter Sun on 11/25/20.
//

#include "IndexHandler.h"

IndexHandler::IndexHandler(const string &corpusPath){
    this->corpusPath = corpusPath;
    this->metaDataMap = loadMetaData(corpusPath);

}

int IndexHandler::createIndex(){
    this->totalFilesLoaded = 0;
    this->avgKeyWordsIndexedPerArticle = 0;
    this->topStemmed50Words.clear();
    this->top50OriginalWords.clear();
    this->totalIndexedWords =  0;

    DocumentParser documentParser = DocumentParser(this->corpusPath, this->stopWordFile, this->metaDataMap);
    documentParser.maxFilesToLoad = this->maxFilesToLoad;
    documentParser.parse(this->keyWordIndex);

    this->totalFilesLoaded = documentParser.totalFilesLoaded;
    this->avgKeyWordsIndexedPerArticle = documentParser.avgKeyWordsIndexedPerArticle;
    this->topStemmed50Words = documentParser.top50StemmedWords;
    this->top50OriginalWords = documentParser.top50OriginalWords;
    this->totalIndexedWords =  this->keyWordIndex.count();
    return this->keyWordIndex.count();
}

IndexNodeData* IndexHandler::searchByKeyword(const string &keyWord){
    IndexNodeData searchQuery;
    searchQuery.keyWord = keyWord;
    DSAvlNode<IndexNodeData> *node = this->keyWordIndex.search(searchQuery);
    if(node == nullptr){
        return nullptr;
    }else {
        return &node->element;
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

