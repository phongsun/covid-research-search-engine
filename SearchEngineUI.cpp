//
// Created by Peter Sun on 11/24/20.
//

#include "SearchEngineUI.h"

using namespace std;

SearchEngineUI::SearchEngineUI(IndexHandler *indexHandler) {
    bool isDone = false;
    while(isDone == false) {
        this->indexHandler = indexHandler;
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
            clearIndex(this->indexHandler->keyWordIndex);
        } else if (choice == 2) {
            this->indexHandler->createIndex();
        } else if (choice == 3) {

        } else if (choice == 4) {
            string keyword;
            cout << "Choose your keyword: ";
            cin >> keyword;
            searchByKeyword(keyword);


        }
    }
}

void SearchEngineUI::clearIndex(DSAvlTree<IndexNodeData> &keywordIndex) {
    DSAvlTree<IndexNodeData> newTree;
    cout << "Size of new tree: " << newTree.count() << endl;
    //this->keywordIndex = newTree;
    cout << "Size of cleared tree: " << newTree.count() << endl;
}

void SearchEngineUI::parseCorpus(DSAvlTree<IndexNodeData> &keywordIndex, string corpusDir){
    int keywordsParsed = this->indexHandler->createIndex();
    cout << "Index has been created and the number of keywords is " << keywordsParsed << endl;


}

void SearchEngineUI::searchByKeyword(const string &keyword){
    unordered_map<string, unsigned int> result;
    result = this->indexHandler->searchByKeyword(keyword);
    for(auto d : result){
        cout << "documentID: " << d.first << endl;
        cout << "Term Frequency: " << d.second << endl;
    }
}

void SearchEngineUI::printStatistics(){
    cout << "Total number articles indexed: " << endl;
    cout << "Average number of words indexed per article: " << endl;
    cout << "Total number of unique authors: " << endl;
    cout << "Top 50 most frequent words: " << endl;
}