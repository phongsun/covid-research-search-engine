//
// Created by Peter Sun on 11/19/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
#define SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H

#include <string>
#include <unordered_map>
#include <math.h>

using namespace std;

class IndexNodeData {
public:
    IndexNodeData();
    bool operator== (const IndexNodeData& rhs) const;
    bool operator< (const IndexNodeData& rhs) const;
    bool operator> (const IndexNodeData& rhs) const;

    string keyWord;
    unordered_map<string, unsigned int> invertedWordFreq;
    //IDF: Inverse Document Frequency, which measures how important a term is.
    // While computing TF, all terms are considered equally important.
    // However it is known that certain terms, such as "is", "of", and "that",
    // may appear a lot of times but have little importance.
    // Thus we need to weigh down the frequent terms while scale up the rare ones,
    // by computing the following:
    double idf;

    // IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
    void calculateIdf(unsigned int totalArticlesIndexed);
};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
