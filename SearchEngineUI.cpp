//
// Created by Peter Sun on 11/24/20.
//

#include "SearchEngineUI.h"

using namespace std;

SearchEngineUI::SearchEngineUI(QueryProcessor *queryProcessor) {

    bool isDone = false;
    this->queryProcessor = queryProcessor;
    while(isDone == false) {
        cout << "Welcome to the COVID-19 search engine!" << endl;
        cout << "1) Clear the index" << endl;
        cout << "2) Parse corpus" << endl;
        cout << "3) Open existing index file" << endl;
        cout << "4) Search for a keyword" << endl;
        cout << "5) Print basic statistics" << endl;
        cout << "6) Quit" << endl;
        int choice;
        cin >> choice;
        if (choice == 1) {
            clearIndex();
        } else if (choice == 2) {
            int keywordsParsed = this->queryProcessor->createIndex();
        } else if (choice == 3) {

        } else if (choice == 4) {
            string keyword;
            cout << "Choose your keyword: ";
            cin >> keyword;
            searchByKeyword(keyword);


        }
    }
}

void SearchEngineUI::clearIndex() {
    //DSAvlTree<IndexNodeData> newTree;
    //cout << "Size of new tree: " << newTree.count() << endl;
    //this->keywordIndex = newTree;
    //cout << "Size of cleared tree: " << newTree.count() << endl;
}


void SearchEngineUI::searchByKeyword(const string &queryString){
    set<string> result;
    result = this->queryProcessor->search(queryString);
    for(auto d : result){
        cout << d << endl;
    }
}

void SearchEngineUI::printStatistics(){
    cout << "Total number articles indexed: " << endl;
    cout << "Average number of words indexed per article: " << endl;
    cout << "Total number of unique authors: " << endl;
    cout << "Top 50 most frequent words: " << endl;
}