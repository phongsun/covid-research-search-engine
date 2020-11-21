#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DSAvlTree.h"
#include "DocumentParser.h"
#include "IndexNodeData.h"

using namespace std;

int main(int argc, char** argv) {

    string searchPhrase = argv[1];
    transform (searchPhrase.begin(), searchPhrase.end(), searchPhrase.begin(), ::tolower);
    Porter2Stemmer::stem(searchPhrase);

    string corpusDir = argv[2];

    DSAvlTree<IndexNodeData> invertedIndexTree;

    //Catch::Session().run();
    DocumentParser documentParser = DocumentParser(corpusDir, "./stop_words.txt");

    //std::chrono::milliseconds parseTime(0);
    //auto t1 = chrono::high_resolution_clock::now();
    documentParser.parse(invertedIndexTree);
    //auto t2 = chrono::high_resolution_clock::now();
    //parseTime = duration_cast<std::chrono::milliseconds>(t2 - t1);
    //cout << "Milliseconds that parsing takes: " << parseTime.count() << endl;
    IndexNodeData searchQuery;
    searchQuery.keyWord = searchPhrase;
    DSAvlNode<IndexNodeData> *node = invertedIndexTree.search(searchQuery);
    //cout << "Number of keywords in the AvlTree index: " << invertedIndexTree.count() << endl;
    if(node == nullptr){
        cout << "Not found" << endl;
    }else{
        for(auto titleAndFreq : node->element.invertedFreq){
            //cout <<  "The word \"" << searchPhrase << "\" appears " << titleAndFreq.second << " times in article ID \"" << titleAndFreq.first << "\"" <<endl;
            cout << titleAndFreq.first << endl;
        }
    }

    return 0;
}
