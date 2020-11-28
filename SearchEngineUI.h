//
// Created by Peter Sun on 11/24/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
#define SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
#include <iostream>
#include "DSAvlTree.h"
#include "IndexNodeData.h"
#include "DocumentParser.h"
#include "QueryProcessor.h"

class SearchEngineUI {
public:
    SearchEngineUI(QueryProcessor *queryProcessor);
    // option 1
    void clearIndex();

    // option 2
    void parseCorpus(DSAvlTree<IndexNodeData> &keywordIndex, string corpusDir);

    // option 3
    void openPersistenceFile();

    // option 4
    void searchByKeyword(const string &queryString);

    // option 5
    void printStatistics();

private:
    QueryProcessor *queryProcessor;
};


#endif //SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
