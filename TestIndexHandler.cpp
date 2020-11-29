//
// Created by Peter Sun on 11/28/20.
//

#include "catch.hpp"
#include "IndexHandler.h"

TEST_CASE("IndexHandler", "IndexHandler") {
    SECTION("Testing index stats") {
        string corpusDir = "../test_data";

        IndexHandler *ih = new IndexHandler(corpusDir);
        ih->createIndex();

        /*cout << "------ " << ih->totalArticlesIndexed << endl;
        cout << "------ " << ih->avgWordsIndexedPerArticle << endl;
        cout << "-----" << ih->totalWordsIndexed << endl;
        cout << "-----" << ih->totalUniqueAuthors << endl;
        int i = 0;
        for (int i = 0; i < 50; i++) {

            cout << ih->top50OriginalWordsData[i].first << " - " << ih->top50OriginalWordsData[i].second << " ^^^";
            cout << ih->topStemmed50WordsData[i].first << " + " << ih->topStemmed50WordsData[i].second << endl;

        }*/
        REQUIRE(ih->totalArticlesIndexed == 100);
        REQUIRE(ih->avgWordsIndexedPerArticle == 594);
        REQUIRE(ih->totalWordsIndexed == 14340);
        REQUIRE(ih->totalUniqueAuthors == 736);
        REQUIRE(ih->top50OriginalWordsData.size() == 50);
        REQUIRE(ih->top50OriginalWordsData[0].second.compare("cells") == 0);
        REQUIRE(ih->top50OriginalWordsData[49].second.compare("including") == 0);
        REQUIRE(ih->topStemmed50WordsData.size() == 50);
        REQUIRE(ih->topStemmed50WordsData[0].second.compare("cell (cells cell ) ") == 0);
        REQUIRE(ih->topStemmed50WordsData[49].second.compare("perform (performed performing performance perform performs performances performer ) ") == 0);

        delete ih;
    }
}