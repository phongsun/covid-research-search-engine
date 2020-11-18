//
// Created by Peter Sun on 11/14/20.
//

#ifndef SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
#define SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include "simdjson.h"

using namespace std;

class DocumentParser {
public:
    void parse(const string &path);
};


#endif //SEARCH_ENGINE_PETER_SUN_DOCUMENTPARSER_H
