//
// Created by Peter Sun on 11/25/20.
//

#include "IndexHandler.h"

IndexHandler::IndexHandler(const string &corpusPath, const unordered_map<string, ArticleMetaData> &metaDataMap){
    this->corpusPath = corpusPath;
    this->metaDataMap = metaDataMap;

}

int IndexHandler::createIndex(){

    DocumentParser documentParser = DocumentParser(this->corpusPath, this->stopWordFile, this->metaDataMap);
    documentParser.parse(this->keyWordIndex);
    return this->keyWordIndex.count();
}

unordered_map<string, unsigned int> IndexHandler::searchByKeyword(const string &keyWord){
    IndexNodeData searchQuery;
    searchQuery.keyWord = keyWord;
    DSAvlNode<IndexNodeData> *node = this->keyWordIndex.search(searchQuery);
    return node->element.invertedTermFreq;

}