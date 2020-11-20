#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DSAvlTree.h"
#include "DocumentParser.h"
#include "IndexNodeData.h"

using namespace std;

int main(int argc, char** argv) {
    string corpusDir = argv[1];
    string searchPhrase = argv[2];
    transform (searchPhrase.begin(), searchPhrase.end(), searchPhrase.begin(), ::tolower);

    DSAvlTree<IndexNodeData> invertedIndexTree;

    //Catch::Session().run();
    DocumentParser documentParser = DocumentParser(corpusDir, "./stop_words.txt");
    documentParser.parse(invertedIndexTree);
    IndexNodeData searchQuery;
    searchQuery.keyWord = searchPhrase;
    DSAvlNode<IndexNodeData> *node = invertedIndexTree.search(searchQuery);
    cout << "Number of keywords in the AvlTree index: " << invertedIndexTree.count() << endl;
    if(node == nullptr){
        cout << "Not found" << endl;
    }else{
        for(auto titleAndFreq : node->element.invertedFreq){
            cout <<  "The word \"" << searchPhrase << "\" appears " << titleAndFreq.second << " times in \"" << titleAndFreq.first << "\"" <<endl;
        }
    }

    return 0;
}
