//
// Created by Peter Sun on 11/14/20.
//

#ifndef SEARCHENGINE_PAPERDATA_H
#define SEARCHENGINE_PAPERDATA_H

#include <string>
#include <set>
#include <map>
#include <vector>

using namespace std;

class ArticleData {
public:
    ArticleData(string documentID);

    string documentID;
    set<string> authorLastNames;

    map<std::string, size_t> keyWordList;
};


#endif //SEARCHENGINE_PAPERDATA_H
