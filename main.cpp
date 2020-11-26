#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DSAvlTree.h"
#include "DocumentParser.h"
#include "IndexNodeData.h"
#include "SearchEngineUI.h"
#include "math.h"
#include "IndexHandler.h"

using namespace std;

int main(int argc, char** argv) {
    std::chrono::milliseconds parseTime(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> t1 = chrono::high_resolution_clock::now();

    string searchPhrase = argv[1];
    transform (searchPhrase.begin(), searchPhrase.end(), searchPhrase.begin(), ::tolower);
    Porter2Stemmer::stem(searchPhrase);

    string corpusDir = argv[2];

    IndexHandler *indexHandler = new IndexHandler(corpusDir);
    SearchEngineUI searchEngineUi = SearchEngineUI(indexHandler);
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
