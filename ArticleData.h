//
// Created by Peter Sun on 11/14/20.
//

#ifndef SEARCHENGINE_PAPERDATA_H
#define SEARCHENGINE_PAPERDATA_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class ArticleData {
public:
    ArticleData(string documentID);

    string documentID;

    unordered_set<string> authorLastNames;

    unordered_map<std::string, unsigned int> keyWordList;
};


#endif //SEARCHENGINE_PAPERDATA_H
