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

using namespace std;

class DocumentParser {
public:
    DocumentParser(const string &corpusPath, const string &stopwordPath);
    void parse(DSAvlTree<IndexNodeData> &keywordIndex);
    unordered_set<string> loadStopWords(const string &filePath);
    inline void addArticleToKeywordIndex(DSAvlTree<IndexNodeData> &avlTree, const ArticleData &articleData);
private:
    string corpusPath;
    string stopwordPath;
};


#endif //SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
