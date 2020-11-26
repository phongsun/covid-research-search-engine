//
// Created by Peter Sun on 11/25/20.
//

#include "IndexHandler.h"

IndexHandler::IndexHandler(string corpusPath){
    this->corpusPath = corpusPath;

}

int IndexHandler::createIndex(){

    DocumentParser documentParser = DocumentParser(this->corpusPath, this->stopWordFile, "metadata-cs2341.csv");
    this->metaDataMap = documentParser.metaDataMap;
    documentParser.parse(this->keyWordIndex);
    return this->keyWordIndex.count();
}

unordered_map<string, unsigned int> IndexHandler::searchByKeyword(const string &keyWord){
    IndexNodeData searchQuery;
    searchQuery.keyWord = keyWord;
    DSAvlNode<IndexNodeData> *node = this->keyWordIndex.search(searchQuery);
    return node->element.invertedTermFreq;

}