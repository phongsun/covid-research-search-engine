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
    SearchEngineUI(QueryProcessor *input);

    void run();

    void displayMainMenu(int &mainMenuChoice);

    void clearScreen();

    // option 1
    void clearIndex();

    // option 2
    void parseCorpus();

    // option 3
    void openPersistenceFile();

    // option 4
    void search();
    void displayAbstract(const QueryResultData &resultData);

    // option 5
    void printStatistics();

private:
    QueryProcessor *queryProcessor;
};


#endif //SEARCH_ENGINE_PETER_SUN_SEARCHENGINEUI_H
