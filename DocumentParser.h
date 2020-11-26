//
// Created by Peter Sun on 11/14/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
#define SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
#include <string>
#include <iostream>
#include <unordered_set>
#include <dirent.h>
#include <fstream>
#include "simdjson.h"
#include "ArticleData.h"
#include "porter2_stemmer.h"
#include "DSAvlTree.h"
#include "IndexNodeData.h"
#include "ArticleMetaData.h"

using namespace std;

class DocumentParser {
public:
    DocumentParser(const string &corpusPath, const string &stopwordPath, const string &metaDataFileName);
    void parse(DSAvlTree<IndexNodeData> &keywordIndex);
    inline void loadMetaData();
    inline unordered_set<string> loadStopWords(const string &filePath);
    inline void addArticleToKeywordIndex(DSAvlTree<IndexNodeData> &avlTree, const ArticleData &articleData);
    DSAvlTree<IndexNodeData> finalIndex;

    string corpusPath;
    string stopwordPath;
    string metaDataFileName;

    unordered_map<string, ArticleMetaData> metaDataMap; // constains full text articles and publishing time
};


#endif //SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
