//
// Created by Peter Sun on 11/14/20.
//

#include <dirent.h>
#include "DocumentParser.h"

void DocumentParser::parse(const string &path){
    int i = 0;
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        return;
    }

    simdjson::dom::parser parser;
    while ((entry = readdir(dir)) != NULL){
        string fullPath = path + "/" + entry->d_name;
        // .json is only one type of file in the folder out of various types
        if (fullPath.find(".json") != string::npos) {
            i++;
            simdjson::dom::element paper = parser.load(fullPath);
            string_view title = paper["metadata"]["title"].get_string();
            cout << "Title: " << title << endl;
        }

        if(i > 3){
            return;
        }
    }
    cout << "number of files: " << i << endl;
    closedir(dir);

}