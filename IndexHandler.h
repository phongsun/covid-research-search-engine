//
// Created by Peter Sun on 11/25/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_INDEXHANDLER_H
#define SEARCH_ENGINE_PETER_SUN_INDEXHANDLER_H
#include <string>
#include <unordered_map>
#include "DocumentParser.h"
#include "DSAvlTree.h"
#include "IndexNodeData.h"
#include "ArticleMetaData.h"

using namespace std;

class IndexHandler {
public:
    IndexHandler(const string &corpusPath, const unordered_map<string, ArticleMetaData> &metaDataMap);
    int createIndex();
    unordered_map<string, unsigned int> searchByKeyword(const string &keyWord);
    unordered_map<string, ArticleMetaData> metaDataMap;
private:
    DSAvlTree<IndexNodeData> keyWordIndex;

    string corpusPath;
    const string stopWordFile = "./stop_words.txt";
    const string keyWordIndexFile = "./keyword_index.txt";
    const string authorIndexFile = "./author_index.txt";
};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXHANDLER_H
