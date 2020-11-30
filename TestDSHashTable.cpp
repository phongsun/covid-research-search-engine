//
// Created by Peter Sun on 11/30/20.
//
#include "catch.hpp"
#include "DSHashTable.h"

TEST_CASE("DSHashTable", "[DSHashTable]") {
    SECTION("Insert") {
        DSHashTable<string, string> testTable(1);
        REQUIRE(testTable.count() == 0);
        for (int i = 0; i < 1000; i++){
            string k = "k" + to_string(i);
            string v = "v" + to_string(i);
            pair<string, string> e(k, v);
            testTable.insert(e);
        }
        REQUIRE(testTable.count() == 1000);

        DSHashTable<string, string>::Iterator iter = testTable.find("k33");
        REQUIRE((*iter).second.compare("v33") == 0);
    }

    SECTION("Erase a entry") {
        DSHashTable<string, string> testTable(1);
        REQUIRE(testTable.count() == 0);
        for (int i = 0; i < 1000; i++){
            string k = "k" + to_string(i);
            string v = "v" + to_string(i);
            pair<string, string> e(k, v);
            testTable.insert(e);
        }
        REQUIRE(testTable.count() == 1000);

        DSHashTable<string, string>::Iterator iter = testTable.find("k33");
        REQUIRE((*iter).second.compare("v33") == 0);

        testTable.erase("k33");
        REQUIRE(testTable.count() == 999);
        // should no longer be found
        REQUIRE(testTable.find("k33") == testTable.end());
    }

    SECTION("Erase HashTable") {
        DSHashTable<string, string> testTable(1);
        REQUIRE(testTable.count() == 0);
        for (int i = 0; i < 1000; i++){
            string k = "k" + to_string(i);
            string v = "v" + to_string(i);
            pair<string, string> e(k, v);
            testTable.insert(e);
        }
        REQUIRE(testTable.count() == 1000);

        DSHashTable<string, string>::Iterator iter = testTable.find("k33");
        REQUIRE((*iter).second.compare("v33") == 0);

        testTable.erase();
        REQUIRE(testTable.count() == 0);
    }
}
