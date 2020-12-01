//
// Created by Peter Sun on 11/19/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
#define SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H

#include <string>
#include <unordered_map>
#include <math.h>
#include "simdjson.h"

using namespace std;

class IndexNodeData {
public:
    IndexNodeData();
    bool operator== (const IndexNodeData& rhs) const;
    bool operator< (const IndexNodeData& rhs) const;
    bool operator> (const IndexNodeData& rhs) const;

    string toJsonString() const; // convert the object to a json string to serialize

    // IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
    void calculateIdf(unsigned int totalArticlesIndexed);

    // overloading << for ostream
    // this is used by DSAvlTree.serialize() to save json representation to the output stream
    friend ostream& operator<<(ostream& os, const IndexNodeData& nodeData) {
        os << nodeData.toJsonString();
        return os;
    }

    // overloading >> for istream
    // this is used by DSAvlTree.deserialize() to parse a json string
    // the parsed json elements are used to populate the data parameter
    friend istream& operator>> (istream& is, IndexNodeData& data) {
        std::string s(std::istreambuf_iterator<char>(is), {});

        // use json parser to get data points from the json string
        // and populate data output parameter which is a type of IndexNodeData
        simdjson::dom::parser jsonParser;
        simdjson::dom::element doc = jsonParser.parse(s);
        data.keyWord = doc["o"]["w"];
        for(auto wordCount : doc["o"]["c"]){
            string_view k_v = wordCount["k"].get_string();
            string k = {k_v.begin(), k_v.end()};
            string_view v_v = wordCount["v"].get_string();
            string v = {v_v.begin(), v_v.end()};
            int vI32 = stoi(v);
            data.invertedWordFreq.insert({k, vI32});
        }

        return is;
    }

    string keyWord;
    unordered_map<string, unsigned int> invertedWordFreq;
    //IDF: Inverse Document Frequency, which measures how important a term is.
    // While computing TF, all terms are considered equally important.
    // However it is known that certain terms, such as "is", "of", and "that",
    // may appear a lot of times but have little importance.
    // Thus we need to weigh down the frequent terms while scale up the rare ones,
    // by computing the following:
    double idf;

};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
