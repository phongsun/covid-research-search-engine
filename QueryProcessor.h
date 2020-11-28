//
// Created by Peter Sun on 11/27/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_QUERYPROCESSOR_H
#define SEARCH_ENGINE_PETER_SUN_QUERYPROCESSOR_H
#include <string>
#include <unordered_map>
#include <set>
#include "ArticleMetaData.h"
#include "IndexHandler.h"

using namespace std;


class QueryProcessor {
public:
    QueryProcessor(IndexHandler *indexHandler);
    unsigned int createIndex();
    set<string> search(string queryString);
private:
    IndexHandler *indexHandler;
};


#endif //SEARCH_ENGINE_PETER_SUN_QUERYPROCESSOR_H
