//
// Created by Peter Sun on 11/28/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_QUERYRESULTDATA_H
#define SEARCH_ENGINE_PETER_SUN_QUERYRESULTDATA_H

#include <ctime>
#include <string>

using namespace std;

class QueryResultData {
public:
    time_t getPubDate() const;

    // these operators contain the ranking algorithm
    QueryResultData(){}
    bool operator== (const QueryResultData& rhs) const;
    bool operator< (const QueryResultData& rhs) const;
    bool operator> (const QueryResultData& rhs) const;

    string keyword;
    string documentId;
    unsigned int tf;
    unsigned int idf;
    string publicationDate;
    string title;
    string authorString;
    string abstract;
};


#endif //SEARCH_ENGINE_PETER_SUN_QUERYRESULTDATA_H
