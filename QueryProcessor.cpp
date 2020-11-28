//
// Created by Peter Sun on 11/27/20.
//

#include "QueryProcessor.h"

QueryProcessor::QueryProcessor(IndexHandler *indexHandler){
    this->indexHandler = indexHandler;
}

unsigned int QueryProcessor::createIndex(){
    return this->indexHandler->createIndex();
}

set<string> QueryProcessor::search(string queryString){
    set<string> retVal;
    unordered_map<string, unsigned int> documentIdAndIdfMap = this->indexHandler->searchByKeyword(queryString);
    string articleInfo = "";
    for(auto documentIdandIdf: documentIdAndIdfMap){
        string documentId = documentIdandIdf.first;
        ArticleMetaData articleMetaData = indexHandler->metaDataMap[documentId];
        string articleInfo = articleMetaData.publicationDate + " " + articleMetaData.title;
        retVal.insert(articleInfo);
    }
    return retVal;
}