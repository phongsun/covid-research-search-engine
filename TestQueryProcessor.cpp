//
// Created by Peter Sun on 11/27/20.
//

#include "catch.hpp"
#include "QueryProcessor.h"

TEST_CASE("QueryProcessor", "QueryProcessor"){
    SECTION("Parsing a keyword only scenario"){

        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("covid");

        REQUIRE(testVal[0].size() == 1);
        REQUIRE(testVal[1].size() == 0);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[0][0].compare("covid") == 0);
        REQUIRE(testVal[3][0].compare("NONE") == 0);

    }

    SECTION("Parsing multiple keywords connected by AND"){
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

    SECTION("Parsing author input only"){
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

    SECTION("Parsing multiple keywords connected by OR"){
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

    SECTION("Parsing multiple keywords connected by AND with a keyword excluded by NOT"){
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

    SECTION("Parsing keyword excluded by NOT"){
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

    SECTION("Search with single keyword"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        /*cout << "------ " << ih->totalArticlesIndexed << endl;
        cout << "------ " << ih->avgWordsIndexedPerArticle << endl;
        cout << "-----" << ih->totalWordsIndexed << endl;
        int i = 0;
        for (int i = 0; i < 50; i++) {

            cout << ih->top50OriginalWordsData[i].first << " - " << ih->top50OriginalWordsData[i].second << " ^^^";
            cout << ih->topStemmed50WordsData[i].first << " + " << ih->topStemmed50WordsData[i].second << endl;

        }*/
        string queryString = "congestion";
        ih->searchByKeyword(queryString);
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        for(auto searchResult: searchResults){
            //cout <<searchResult.weight << "  |  "<<searchResult.idf << "  |  " <<searchResult.tf << "  |  " << searchResult.documentId << "  |  " <<searchResult.title << endl;
        }
    }

    SECTION("Search with AND only"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "AND cell covid patients inflame Pediatric";
        ih->searchByKeyword(queryString);
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        for(auto searchResult: searchResults){
            cout << searchResult.weight << "  |  " << searchResult.documentId << "  |  " <<searchResult.title << endl;
        }
    }

    SECTION("Search with OR only"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "OR covid congestion";
        ih->searchByKeyword(queryString);
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        for(auto searchResult: searchResults){
            //cout << searchResult.weight << "  |  " << searchResult.documentId << "  |  " <<searchResult.title << endl;
        }
    }
}