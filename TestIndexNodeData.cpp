//
// Created by Peter Sun on 11/30/20.
//

#include <iostream>
#include "catch.hpp"
#include "IndexNodeData.h"
#include "simdjson.h"

TEST_CASE("IndexNodeData", "[IndexNodeData]") {
    SECTION( "Test IndexNodeData to JSON Conversion" ) {
        // create test object
        IndexNodeData inputData;
        inputData.keyWord = "covid";
        unordered_map<string, unsigned int> invertedWordCount;
        inputData.invertedWordFreq["peter"] = 1000;
        inputData.invertedWordFreq["hong"] = 50;
        inputData.invertedWordFreq["sun"] = 3000;
        // convert to json
        string testJson = inputData.toJsonString();

        simdjson::dom::parser jsonParser;
        simdjson::dom::element doc = jsonParser.parse(testJson);

        string_view wordV = doc["o"]["w"].get_string();
        string wordS = {wordV.begin(), wordV.end()};
        //{"o": {"w":"covid", "c": [{"k":"sun","v":"3000"},{"k":"hong","v":"50"},{"k":"peter","v":"1000"}]}}
        REQUIRE(wordS.compare("covid") == 0);

        for(auto wordCount : doc["o"]["c"]) {
            string_view k_v = wordCount["k"].get_string();
            string k = {k_v.begin(), k_v.end()};
            string_view v_v = wordCount["v"].get_string();
            string v = {v_v.begin(), v_v.end()};
            int vI32 = stoi(v);

            //{"o": {"w":"covid", "c": [{"k":"sun","v":"3000"},{"k":"hong","v":"50"},{"k":"peter","v":"1000"}]}}
            // k must be one of these values
            bool expectedKeys = k.compare("sun") == 0
                                ||k.compare("hong") == 0
                                ||k.compare("peter") == 0;
            REQUIRE(expectedKeys == true);

            // values must match the keys
            if (k.compare("sun") == 0) {
                REQUIRE(vI32 == 3000);
            }
            if (k.compare("hong") == 0) {
                REQUIRE(vI32 == 50);
            }
            if (k.compare("peter") == 0) {
                REQUIRE(vI32 == 1000);
            }
        }
    }
}