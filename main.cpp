#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DSAvlTree.h"
#include "DocumentParser.h"
#include "IndexNodeData.h"
#include "SearchEngineUI.h"
#include "math.h"
#include "IndexHandler.h"
#include "QueryProcessor.h"

using namespace std;

unordered_map<string, ArticleMetaData> loadMetaData(const string &corpusPath){
    unordered_map<string, ArticleMetaData> metaDataMap;

    string filePath;

    filePath = corpusPath
               + (corpusPath.c_str()[corpusPath.size() - 1] == '/' ? "" : "/" )
               + "metadata-cs2341.csv";
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {

            // tokenize the line by comma
            // only take the full text articles (16)
            // map has key of documentId (1) and value of publish_time (9)
            std::vector<std::string> result;
            std::stringstream lineStream(line);
            std::string cell;

            // read full line
            const char *mystart = line.c_str();    // prepare to parse the line - start is position of begin of field
            bool instring{false};
            for (const char *p = mystart; *p; p++) {  // iterate through the string
                if (*p == '"')                        // toggle flag if we're btw double quote
                    instring = !instring;
                else if (*p == ',' && !instring) {    // if comma OUTSIDE double quote
                    result.push_back(string(mystart, p - mystart));  // keep the field
                    mystart = p + 1;                    // and start parsing next one
                }
            }
            result.push_back(string(mystart));   // last field delimited by end of line instead of comma
            ArticleMetaData metaData;
            metaData.publicationDate = result[9];
            metaData.title = result[3];
            metaData.abstract = result[8];


            metaDataMap[result[1]] = metaData;
        }
        file.close();

    } else {
        cout<< "Failed to load meta data file. Cannot find " << filePath << endl;
    }

    return metaDataMap;
}


int main(int argc, char** argv) {

    std::chrono::milliseconds parseTime(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> t1 = chrono::high_resolution_clock::now();

    string searchPhrase = argv[1];
    Porter2Stemmer::stem(searchPhrase);

    string corpusDir = argv[2];

    unordered_map<string, ArticleMetaData> metaDataMap = loadMetaData(corpusDir);

    IndexHandler *indexHandler = new IndexHandler(corpusDir, metaDataMap);

    QueryProcessor *queryProcessor = new QueryProcessor(indexHandler);

    SearchEngineUI searchEngineUi = SearchEngineUI(queryProcessor);
    //Catch::Session().run();

    /*IndexNodeData searchQuery;
    searchQuery.keyWord = searchPhrase;
    DSAvlNode<IndexNodeData> *node = invertedIndexTree.search(searchQuery);
    //cout << "Number of keywords in the AvlTree index: " << invertedIndexTree.count() << endl;
    if(node == nullptr){
        cout << "Not found" << endl;
    }else{
        for(auto titleAndFreq : node->element.invertedTermFreq){
            //cout <<  "The word \"" << searchPhrase << "\" appears " << titleAndFreq.second << " times in article ID \"" << titleAndFreq.first << "\"" <<endl;
            //cout << titleAndFreq.first << endl;
        }
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> t2 = chrono::high_resolution_clock::now();
    parseTime = (std::chrono::duration_cast<std::chrono::milliseconds>)(t2 - t1);
    cout << endl << "--------------- stats ----------------" << endl;
    int numberOfNodes = invertedIndexTree.count();
    cout << "Keywords (# of nodes) in index (tree): " << numberOfNodes << endl;
    cout << "AVL Tree Root Height: " << invertedIndexTree.height() << endl;
    cout << "Milliseconds that the program takes: " << parseTime.count() << endl;*/

    return 0;
}
