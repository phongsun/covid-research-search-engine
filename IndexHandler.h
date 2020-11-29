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
    IndexHandler(const string &corpusPath);
    int createIndex();
    IndexNodeData* searchByKeyword(const string &keyWord);
    unordered_map<string, ArticleMetaData> metaDataMap;

    int maxFilesToLoad = -1;
    unsigned int totalArticlesIndexed = 0;
    unsigned int avgWordsIndexedPerArticle = 0;
    unsigned int totalWordsIndexed = 0;
    int totalUniqueAuthors = 0;
    std::vector<string> topStemmed50Words;
    std::vector<string> top50OriginalWords;
    std::vector<std::pair<int,string>> topStemmed50WordsData;
    std::vector<std::pair<int,string>> top50OriginalWordsData;
private:
    DSAvlTree<IndexNodeData> keyWordIndex;
    unordered_map<string, ArticleMetaData> loadMetaData(const string &corpusPath);
    string corpusPath;
    const string stopWordFile = "./stop_words.txt";
    const string keyWordIndexFile = "./keyword_index.txt";
    const string authorIndexFile = "./author_index.txt";

};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXHANDLER_H
