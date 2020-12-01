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
#include "simdjson.h"

using namespace std;

class IndexHandler {
public:
    IndexHandler(const string &corpusPath);
    int createIndex();
    bool persistKeywordIndex();
    bool restoreKeywordIndex();
    bool persistAuthorIndex();
    bool restoreAuthorIndex();
    bool restoreIndex();

    IndexNodeData* searchByKeyword(const string &keyWord);
    unordered_set<string> searchByAuthor(const string &author);

    void clearIndex();
    bool isIndexEmpty();

    unordered_map<string, ArticleMetaData> metaDataMap;

    int maxFilesToLoad = -1;
    unsigned int totalArticlesIndexed = 0;
    unsigned int avgWordsIndexedPerArticle = 0;
    unsigned int totalWordsIndexed = 0;
    int totalUniqueAuthors = 0;
    std::vector<std::pair<int,string>> topStemmed50WordsData;
    std::vector<std::pair<int,string>> top50OriginalWordsData;

    unordered_map<string, ArticleMetaData> loadMetaData(const string &corpusPath);

    DSAvlTree<IndexNodeData>::Iterator getAvlTreeIterator() { return this->keyWordIndex->iterator(); }
    DSHashTable<string, unordered_set<string>>* authorX() { return this->authorIndex; }
    void setPersistentDir(const string &input) { this->persistentDir = input; }
    string getPersistentDir() { return this->persistentDir; }
    string getKeyWordIndexFilePath() { return this->persistentDir + "/" + this->keyWordIndexFile; }
    string getAuthorIndexFilePath() { return this->persistentDir + "/" + this->authorIndexFile; }

private:
    string persistentDir = "../persistence";
    const string stopWordFile = "./stop_words.txt";
    const string keyWordIndexFile = "keyword_index.json";
    const string authorIndexFile = "author_index.json";

    string corpusPath;
    DSAvlTree<IndexNodeData>* keyWordIndex;
    DSHashTable<string, unordered_set<string>> *authorIndex;
};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXHANDLER_H
