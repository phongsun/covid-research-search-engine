//
// Created by Peter Sun on 11/19/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
#define SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H

#include <string>
#include <unordered_map>

using namespace std;

class IndexNodeData {
public:
    IndexNodeData();
    bool operator== (const IndexNodeData& rhs) const;
    bool operator< (const IndexNodeData& rhs) const;
    bool operator> (const IndexNodeData& rhs) const;

    string keyWord;
    unordered_map<string, unsigned int> invertedTermFreq;
    unsigned int inverseDocFreq; // IDF(t) = log_e(Total number of documents / Number of documents with term t in it).

};


#endif //SEARCH_ENGINE_PETER_SUN_INDEXNODEDATA_H
