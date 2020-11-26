//
// Created by Peter Sun on 11/24/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
#define SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
#include <iostream>
#include "DSAvlTree.h"
#include "IndexNodeData.h"
#include "DocumentParser.h"
#include "IndexHandler.h"

class SearchEngineUI {
public:
    SearchEngineUI(IndexHandler *indexHandler);
    // option 1
    void clearIndex(DSAvlTree<IndexNodeData> &keywordIndex);

    // option 2
    void parseCorpus(DSAvlTree<IndexNodeData> &keywordIndex, string corpusDir);

    // option 3
    void openPersistenceFile();

    // option 4
    void searchByKeyword(const string &keyword);

    // option 5
    void printStatistics();

private:
    IndexHandler *indexHandler;
};


#endif //SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
