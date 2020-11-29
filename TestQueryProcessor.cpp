//
// Created by Peter Sun on 11/27/20.
//

#include "catch.hpp"
#include "QueryProcessor.h"

TEST_CASE("QueryProcessor", "QueryProcessor"){
    SECTION("Testing a keyword only scenario"){

        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("covid");

        REQUIRE(testVal[0].size() == 1);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[3][0].compare("NONE") == 0);

    }

    SECTION("Testing multiple keywords connected by AND"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("AND covid vaccine");

        REQUIRE(testVal[0].size() == 2);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[0][1].compare("vaccine") == 0);
        REQUIRE(testVal[3][0].compare("AND") == 0);
    }

    SECTION("Testing author only"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("AUTHOR robbins sun");

        cout << testVal[1].size() << endl;

        REQUIRE(testVal[0].size() == 0);
        REQUIRE(testVal[1].size() == 2);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[1][0].compare("robbins") == 0);
        REQUIRE(testVal[1][1].compare("sun") == 0);
        REQUIRE(testVal[3][0].compare("NONE") == 0);
    }

    SECTION("Testing multiple keywords connected by OR"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("OR covid vaccine");

        REQUIRE(testVal[0].size() == 2);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[0][1].compare("vaccine") == 0);
        REQUIRE(testVal[3][0].compare("OR") == 0);
    }

    SECTION("Testing multiple keywords connected by AND with a keyword excluded by NOT"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("AND covid vaccine NOT virus");

        REQUIRE(testVal[0].size() == 2);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 1);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[0][1].compare("vaccine") == 0);
        REQUIRE(testVal[2][0].compare("virus") == 0);
        REQUIRE(testVal[3][0].compare("AND") == 0);
    }

    SECTION("Testing keyword excluded by NOT"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("covid vaccine NOT virus");

        REQUIRE(testVal[0].size() == 2);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 1);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[0][1].compare("vaccine") == 0);
        REQUIRE(testVal[2][0].compare("virus") == 0);
        REQUIRE(testVal[3][0].compare("NONE") == 0);
    }

    SECTION("Testing searching"){
        IndexHandler *ih = new IndexHandler("../corpus");
        // load 100 files into the index
        ih->maxFilesToLoad = 100;
        ih->createIndex();

        /*cout << "------ " << ih->totalFilesLoaded << endl;
        cout << "------ " << ih->avgKeyWordsIndexedPerArticle << endl;
        cout << "-----" << ih->totalIndexedWords << endl;
        int i = 0;
        for (int i = 0; i < 50; i++) {

            cout << ih->top50OriginalWords[i].first << " - " << ih->top50OriginalWords[i].second << " ^^^";
            cout << ih->topStemmed50Words[i].first << " + " << ih->topStemmed50Words[i].second << endl;

        }*/
        string queryString = "cell";
        ih->searchByKeyword(queryString);
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        for(auto searchResult: searchResults){
            cout << searchResult.tf << "  |  " << searchResult.documentId << "  |  " <<searchResult.title << endl;
        }
    }

}