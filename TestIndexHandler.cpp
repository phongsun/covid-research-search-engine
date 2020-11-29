//
// Created by Peter Sun on 11/28/20.
//

#include "catch.hpp"
#include "IndexHandler.h"

TEST_CASE("IndexHandler", "IndexHandler") {
    SECTION("Testing index stats") {
        string corpusDir = "../corpus";

        IndexHandler *ih = new IndexHandler(corpusDir);
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
        REQUIRE(ih->totalFilesLoaded <= ih->maxFilesToLoad);
        REQUIRE(ih->avgKeyWordsIndexedPerArticle > 0);
        REQUIRE(ih->totalIndexedWords > 0);
        REQUIRE(ih->top50OriginalWords.size() == 50);
        REQUIRE(ih->topStemmed50Words.size() == 50);

        delete ih;
    }
}