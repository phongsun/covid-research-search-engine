#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DocumentParser.h"
#include "IndexNodeData.h"
#include "SearchEngineUI.h"
#include "IndexHandler.h"
#include "QueryProcessor.h"

//#define TEST_ONLY true;

using namespace std;

int main(int argc, char** argv) {
    string testFlag = argv[1];
    if(testFlag.compare("-testonly") == 0){
        Catch::Session().run();
        return 0;
    }



    string corpusDir = argv[2];

    IndexHandler *indexHandler = new IndexHandler(corpusDir);

    QueryProcessor *queryProcessor = new QueryProcessor(indexHandler);

    SearchEngineUI *searchEngineUi = new SearchEngineUI(queryProcessor);

    searchEngineUi->run();

    delete searchEngineUi;

    return 0;
}
