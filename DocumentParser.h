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
#include <map>

#include "simdjson.h"
#include "ArticleData.h"
#include "porter2_stemmer.h"
#include "DSAvlTree.h"
#include "DSHashTable.h"

#include "IndexNodeData.h"
#include "ArticleMetaData.h"

using namespace std;

class DocumentParser {
public:
    DocumentParser(const string &corpusPath, const string &stopwordPath, const unordered_map<string, ArticleMetaData> &metaDataMap);
    void parse(DSAvlTree<IndexNodeData> *keywordIndex, DSHashTable<string, unordered_set<string>> *authorIndex);
    inline unordered_set<string> loadStopWords(const string &filePath);
    inline void addArticleToKeywordIndex(DSAvlTree<IndexNodeData> *avlTree, const ArticleData &articleData);
    inline void addAuthorsToHashTable(DSHashTable<string, unordered_set<string>> *authorIndex, const ArticleData &input);

    string corpusPath;
    string stopwordPath;

    unordered_map<string, ArticleMetaData> metaDataMap; // constains full text articles and publishing time

    unsigned int totalFilesLoaded = 0;
    unsigned int avgKeyWordsIndexedPerArticle = 0;
    std::vector<std::pair<int,string>> top50StemmedWords;
    std::vector<std::pair<int,string>> top50OriginalWords;
    int totalUniqueAuthors = 0;
    int maxFilesToLoad = -1;
};


#endif //SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
