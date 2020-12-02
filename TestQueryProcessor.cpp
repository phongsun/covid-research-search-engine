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

        REQUIRE(testVal[0].size() == 0);
        REQUIRE(testVal[1].size() == 2);
        REQUIRE(testVal[2].size() == 0);
        REQUIRE(testVal[3].size() == 1);
        REQUIRE(testVal[1][0].compare("robbins") == 0);
        REQUIRE(testVal[1][1].compare("sun") == 0);
        REQUIRE(testVal[3][0].compare("NONE") == 0);
    }

    SECTION("Parsing single word with author"){
        QueryProcessor *queryProcessor = new QueryProcessor(nullptr);

        vector<string>* testVal = queryProcessor->parseQueryString("congestion AUTHOR samy");

        REQUIRE(testVal[queryProcessor->KEYWORD].size() == 1);
        REQUIRE(testVal[queryProcessor->AUTHOR].size() == 1);
        REQUIRE(testVal[queryProcessor->EXCLUSION].size() == 0);
        REQUIRE(testVal[queryProcessor->OP].size() == 1);
        REQUIRE(testVal[queryProcessor->AUTHOR][0].compare("samy") == 0);
        REQUIRE(testVal[queryProcessor->KEYWORD][0].compare("congestion") == 0);
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

    SECTION("Search with NONE"){
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
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 4);
    }

    SECTION("Search with AND only"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "AND cell covid patients inflame Pediatric";
        //string queryString;
        //getline(cin, queryString, '\n');
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 1);
        delete qP;
    }

    SECTION("Search with OR only"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "OR covid congestion";
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 14);
        delete qP;
    }

    SECTION("Search single NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("cell");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 75);

        parsedQuery = qP->parseQueryString("cell NOT covid");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 71);

        for (auto r: searchResults) {
            //cout << r.weight << " |" << r.datePublished << " |" << r.publication << endl;
        }
        delete qP;
    }

    SECTION("OR NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "OR cell congestion";
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 76);

        parsedQuery = qP->parseQueryString("OR cell congestion NOT covid");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 72);
        delete qP;
    }

    SECTION("AND NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "AND cell vaccine";
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 22);

        parsedQuery = qP->parseQueryString("AND cell vaccine NOT disease");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 2);
        delete qP;
    }

    SECTION("AND AUTHOR"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "AND covid congestion";
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 0);

        parsedQuery = qP->parseQueryString("AND cell vaccine AUTHOR wang");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 1);
        delete qP;
    }

    SECTION("OR AUTHOR"){
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();

        string queryString = "OR cell congestion";
        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString(queryString);
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 76);

        parsedQuery = qP->parseQueryString("OR cell congestion AUTHOR wang");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 12);

        parsedQuery = qP->parseQueryString("OR cell congestion covid");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 82);
        delete qP;
    }

    SECTION("NONE AUTHOR"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("congestion");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 4);

        parsedQuery = qP->parseQueryString("congestion AUTHOR samy");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 1);
        delete qP;
    }

    SECTION("NONE AUTHOR NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("cell");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 75);

        parsedQuery = qP->parseQueryString("cell AUTHOR wang");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 12);

        parsedQuery = qP->parseQueryString("cell AUTHOR wang NOT vaccine");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 11);
        delete qP;
    }

    SECTION("AND AUTHOR NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("AND cell virus");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 66);

        parsedQuery = qP->parseQueryString("AND cell virus AUTHOR wang");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 11);

        parsedQuery = qP->parseQueryString("AND cell virus AUTHOR wang NOT vaccine");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 10);
        delete qP;
    }

    SECTION("OR AUTHOR NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("OR cell virus");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 90);

        parsedQuery = qP->parseQueryString("OR cell virus AUTHOR wang");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 13);

        parsedQuery = qP->parseQueryString("OR cell virus AUTHOR wang NOT vaccine");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 12);
        delete qP;
    }

    /*SECTION("AUTHOR NOT"){
        IndexHandler *ih = new IndexHandler("../test_data");
        ih->createIndex();

        QueryProcessor *qP = new QueryProcessor(ih);
        vector<string>* parsedQuery = qP->parseQueryString("AUTHOR wang");
        set<QueryResultData> searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 50);

        parsedQuery = qP->parseQueryString("AUTHOR wang NOT vaccine");
        searchResults = qP->search(parsedQuery[qP->OP][0], parsedQuery[qP->KEYWORD], parsedQuery[qP->EXCLUSION], parsedQuery[qP->AUTHOR]);
        REQUIRE(searchResults.size() == 12);
    }*/
}