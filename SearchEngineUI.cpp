//
// Created by Peter Sun on 11/24/20.
//

#include "SearchEngineUI.h"

using namespace std;

SearchEngineUI::SearchEngineUI(QueryProcessor *input) {
    this->queryProcessor = input;
}

void SearchEngineUI::displayMainMenu(int &mainMenuChoice){
    this->clearScreen();
    cout << "Welcome to the COVID-19 search engine!" << endl;
    cout << "1) Clear the index" << endl;
    cout << "2) Parse corpus" << endl;
    cout << "3) Open existing index file" << endl;
    cout << "4) Search for a keyword" << endl;
    cout << "5) Print basic statistics" << endl;
    cout << "6) Quit" << endl;
    cout << "Enter choice here: " << endl;
    cin >> mainMenuChoice;
    cout << endl;
}

void SearchEngineUI::run(){
    bool isDone = false;
    int mainMenuChoice;

    while(isDone == false) {

        this->displayMainMenu(mainMenuChoice);
        if (mainMenuChoice == 1) { // clear index
            clearIndex();
        } else if (mainMenuChoice == 2) { // create the index
            parseCorpus();
        } else if (mainMenuChoice == 3) {
            cout << "TO BE IMPLEMENTED!!!" << endl << endl;
        } else if (mainMenuChoice == 4) {
            search();


        } else if (mainMenuChoice == 5) {
            printStatistics();
        } else if (mainMenuChoice == 6) {
            cout << "Understood. Have a nice day." << endl;
            isDone = true;
        } else{
            cout << "Please enter a valid choice." << endl << endl;
        }
    }
}

void SearchEngineUI::clearScreen(){
    for(int i = 0; i < 20; i++){
        cout << endl;
    }
}

void SearchEngineUI::clearIndex() {
    // if the index is already empty notify the user
    if(this->queryProcessor->isIndexEmpty()){
        cout << "Index is already clear. Press any key to go back to the main menu." << endl;
        int tmp;
        cin >> tmp;
    }else{ // otherwise confirm with the user
        bool choiceChosen = false;
        while(choiceChosen == false) {
            char choice;
            cout << "Are you sure you want to clear the index? (y/n)" << endl;
            cin >> choice;
            if (choice == 'y') {
                this->queryProcessor->clearIndex();
                cout << "Index cleared!" << endl << endl;
                choiceChosen = true;
            }else if (choice == 'n'){
                cout << "Understood. Have a nice day." << endl;
                choiceChosen = true;
            }else{
                cout << "Please enter a valid mainMenuChoice." << endl;
            }
        }

    }
}

void SearchEngineUI::parseCorpus(){
    this->clearScreen();
    bool createIndex = true;
    // if the index isn't empty then warn the user
    if(!this->queryProcessor->isIndexEmpty()){
        createIndex = false; // dont know if the user wants to create an index or not
        bool choiceChosen = false;
        while(choiceChosen == false) { // in case the user types in an invalid mainMenuChoice
            char choice;
            cout << "The index is not empty. If you continue, the current index will be overwritten. Are you sure? (y/n)"
                 << endl;
            if(choice == 'y'){
                // clear index and give the program the 'thumbs up' to create the index
                this->queryProcessor->clearIndex();
                createIndex = true;
                choiceChosen = true;
            }else if(choice == 'n'){
                // go back to the beginning of the UI
                cout << "Understandable. Have a nice day." << endl;
                choiceChosen = true;
            }else{
                cout << "Please enter a valid (y/n)!" << endl;
            }
        }
    }

    if(createIndex == true){ // create the index
        int noOfKeywords = this->queryProcessor->createIndex();
        printStatistics();
        cout << "Press 1 to continue." << endl;
        int cont;
        cin >> cont;
    }
}

void SearchEngineUI::search(){
    string keyword;
    cout << "Enter your search term: ";
    cin >> keyword;
    vector<string>* parsedQuery = this->queryProcessor->parseQueryString(keyword);
    set<QueryResultData> searchResults = this->queryProcessor->search(
                                         parsedQuery[this->queryProcessor->OP][0],
                                         parsedQuery[this->queryProcessor->KEYWORD],
                                         parsedQuery[this->queryProcessor->EXCLUSION],
                                         parsedQuery[this->queryProcessor->AUTHOR]);
    if(searchResults.size() == 0){
        cout << "Not found. Please enter a different search phrase." << endl;
    }else{
        while(true) {
            cout << "Found " << searchResults.size() << " articles." << endl;
            if (searchResults.size() > 15) {
                cout << "Top 15 results: " << endl;
            }
            int i = 0;
            for (auto d : searchResults) {
                i++;
                cout << i << ". " << d.title << endl;
                cout << "Published at " << d.publication;
                cout << "Published on " << d.datePublished;
                cout << "Published by " << d.authorString << endl;
                if (i == 15) {
                    break;
                }
            }
            cout << "Which search result would you like to see the abstract for? (1-15)" << endl;
            cout << "Or do you want to exit to the main menu? (0)" << endl;
            int abstractOrQuit;
            cin >> abstractOrQuit;
            if (abstractOrQuit >= 1 && abstractOrQuit <= 15) {
                abstractOrQuit--;
                displayAbstract(*(next(searchResults.begin(), abstractOrQuit)));
                cout << "Press q to go back to the main menu or press c to go back to the results" << endl;
                char abstractChoice;
                cin >> abstractChoice;
                if(abstractChoice == 'c'){
                    continue;
                }else{
                    break;
                }
            }else{
                break;
            }
        }
    }

}

void SearchEngineUI::displayAbstract(const QueryResultData &d){
    cout <<  d.title << endl;
    cout << "Published at " << d.publication;
    cout << "Published on " << d.datePublished;
    cout << "Published by " << d.authorString << endl << endl;
    cout << d.abstract << endl;
    cout << "Press any key to return to the menu." << endl;

}

void SearchEngineUI::printStatistics() {
    if (this->queryProcessor->isIndexEmpty()) {
        cout << "Index is empty. Please select choice 2 and then come back." << endl;
    } else {
        cout << "=======================" << endl;
        cout << "=======STATISTICS======" << endl;
        cout << "=======================" << endl << endl;
        cout << "Total articles indexed: " << this->queryProcessor->getTotalArticlesIndexed() << endl;
        cout << "Average number of words indexed per article: " << this->queryProcessor->getAvgWordsIndexedPerArticle()
             << endl;
        cout << "Total words indexed: " << this->queryProcessor->getTotalWordsIndexed() << endl;
        cout << "Total unique authors: " << this->queryProcessor->getTotalUniqueAuthors() << endl;
        cout << "Top 50 most frequent words : ";
        for (auto top50OriginalWord: this->queryProcessor->getTop50OriginalWords()) {
            cout << top50OriginalWord.second << ", ";
        }
        cout << endl << endl;
    }
}