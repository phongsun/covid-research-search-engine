//
// Created by Peter Sun on 11/28/20.
//

#include "catch.hpp"
#include "IndexHandler.h"
#include <unistd.h>
#include <string>

using namespace std;

TEST_CASE("IndexHandler", "IndexHandler") {
    SECTION("Create Keyword Index") {
        string corpusDir = "./test_data";

        IndexHandler *ih = new IndexHandler(corpusDir);
        ih->createIndex();

        /*cout << "------ " << ih->totalArticlesIndexed << endl;
        cout << "------ " << ih->avgWordsIndexedPerArticle << endl;
        cout << "-----" << ih->totalWordsIndexed << endl;
        cout << "-----" << ih->totalUniqueAuthors << endl;
        int i = 0;
        for (int i = 0; i < 50; i++) {

            cout << ih->top50OriginalWordsData[i].first << " - " << ih->top50OriginalWordsData[i].second << " ^^^";
            cout << ih->topWords[i]<< endl;

        }*/
        REQUIRE(ih->totalArticlesIndexed == 100);
        REQUIRE(ih->avgWordsIndexedPerArticle == 594);
        REQUIRE(ih->totalWordsIndexed == 14340);
        REQUIRE(ih->totalUniqueAuthors == 736);
        REQUIRE(ih->top50OriginalWords.size() == 50);
        REQUIRE(ih->top50OriginalWords[0].second.compare("cells") == 0);
        REQUIRE(ih->top50OriginalWords[49].second.compare("including") == 0);
        REQUIRE(ih->topWords.size() == 50);
        REQUIRE(ih->topWords[0].compare("cells") == 0);
        REQUIRE(ih->topWords[49].compare("including") == 0);
        delete ih;
    }

    SECTION("Clear Keyword Index") {

        IndexHandler *ih = new IndexHandler("./test_data");
        ih->createIndex();
        REQUIRE(ih->totalArticlesIndexed == 100);
        REQUIRE(ih->avgWordsIndexedPerArticle == 594);
        REQUIRE(ih->totalWordsIndexed == 14340);
        REQUIRE(ih->totalUniqueAuthors == 736);
        REQUIRE(ih->top50OriginalWords.size() == 50);
        REQUIRE(ih->top50OriginalWords[0].second.compare("cells") == 0);
        REQUIRE(ih->top50OriginalWords[49].second.compare("including") == 0);
        ih->clearIndex();

        REQUIRE(ih->totalArticlesIndexed == 0);
        REQUIRE(ih->avgWordsIndexedPerArticle == 0);
        REQUIRE(ih->totalWordsIndexed == 0);
        REQUIRE(ih->totalUniqueAuthors == 0);
        REQUIRE(ih->top50OriginalWords.size() == 0);
        delete ih;

    }

    SECTION("Search Author Index") {
        IndexHandler *ih = new IndexHandler("./test_data");
        ih->createIndex();

        unordered_set<string> documentIds = ih->searchByAuthor("wang");
        REQUIRE(documentIds.size() == 14);
        delete ih;
    }

    SECTION("Serialize and Deserailize Stats") {
        IndexHandler *ih1 = new IndexHandler("./test_data");
        ih1->createIndex();
        REQUIRE(ih1->persistStats() == true);
        REQUIRE(ih1->totalArticlesIndexed == 100);
        REQUIRE(ih1->avgWordsIndexedPerArticle == 594);
        REQUIRE(ih1->totalWordsIndexed == 14340);
        REQUIRE(ih1->totalUniqueAuthors == 736);
        REQUIRE(ih1->topWords.size() == 50);
        REQUIRE(ih1->topWords[0].compare("cells") == 0);
        REQUIRE(ih1->topWords[49].compare("including") == 0);

        IndexHandler *ihX = new IndexHandler("./test_data");
        ihX->clearIndex();
        REQUIRE(ihX->restoreStats() == true);
        REQUIRE(ihX->totalArticlesIndexed == 100);
        REQUIRE(ihX->avgWordsIndexedPerArticle == 594);
        REQUIRE(ihX->totalWordsIndexed == 14340);
        REQUIRE(ihX->totalUniqueAuthors == 736);
        REQUIRE(ihX->topWords.size() == 50);
        REQUIRE(ihX->topWords[0].compare("cells") == 0);
        REQUIRE(ihX->topWords[49].compare("including") == 0);

        for (int i = 0; i < 50; i++){
            REQUIRE(ih1->topWords[i].compare(ihX->topWords[i]) == 0);
        }
        delete ih1;
        delete ihX;
    }

    SECTION("Serialize and Deserailize Author Index") {

        IndexHandler *ih1 = new IndexHandler("./test_data");
        ih1->createIndex();
        REQUIRE(ih1->persistAuthorIndex() == true);
        DSHashTable<string, unordered_set<string>> *author1 = ih1->authorX();
        DSHashTable<string, unordered_set<string>>::Iterator iter1 = author1->begin();

        IndexHandler *ihX = new IndexHandler("./test_data");
        ihX->clearIndex();
        REQUIRE(ihX->restoreAuthorIndex() == true);
        DSHashTable<string, unordered_set<string>> *authorX = ihX->authorX();

        while (iter1 != author1->end()) {
            pair<string, unordered_set<string>> el_1 = *iter1;
            DSHashTable<string, unordered_set<string>>::Iterator tmpX = authorX->find(el_1.first);
            // the same author name should match in both index
            REQUIRE(tmpX != authorX->end());
            // for the same author, all documents in the list should match
            unordered_set<string>::iterator docIter1 = el_1.second.begin(); // document id iterator
            unordered_set<string> docListX = (*tmpX).second; // document id list in restored index
            unordered_set<string>::iterator docIterXEnd = docListX.end();
            while (docIter1 != el_1.second.end()) {
                string docId1 = *docIter1; // doc ID in from original index
                REQUIRE(docListX.find(docId1) != docIterXEnd); // should find it in restored index
                docIter1++;
            }
            iter1++;
        }

        unordered_set<string> documentIds_1 = ih1->searchByAuthor("wang");
        REQUIRE(documentIds_1.size() == 14);

        unordered_set<string> documentIds_x = ihX->searchByAuthor("wang");
        REQUIRE(documentIds_x.size() == 14);
        delete ih1;
        delete ihX;
    }
}