//
// Created by Peter Sun on 11/24/20.
//

#include "SearchEngineUI.h"

using namespace std;

SearchEngineUI::SearchEngineUI(QueryProcessor *input) {
    clearScreen();
    this->queryProcessor = input;
}

void SearchEngineUI::displayMainMenu(int &mainMenuChoice) {
    while (true) {
        clearScreen();
        cout << "Welcome to the COVID-19 search engine!" << endl;
        cout << "1) Clear the index" << endl;
        cout << "2) Parse corpus" << endl;
        cout << "3) Open existing index file" << endl;
        cout << "4) Search for a keyword" << endl;
        cout << "5) Print basic statistics" << endl;
        cout << "6) Quit" << endl;
        cout << "Enter choice here: " << endl;

        char o = cin.get();
        int c = o - '0';
        if (c >= 1 && c <= 6) {
            mainMenuChoice = c;
            break;
        }
    }
    cout << endl;
}

void SearchEngineUI::run() {
    bool isDone = false;
    int mainMenuChoice;

    while (isDone == false) {

        this->displayMainMenu(mainMenuChoice);
        if (mainMenuChoice == 1) { // clear index
            clearIndex();
        } else if (mainMenuChoice == 2) { // create the index
            parseCorpus();
        } else if (mainMenuChoice == 3) {
            openPersistenceFile();
        } else if (mainMenuChoice == 4) {
            cin.ignore();
            search();
        } else if (mainMenuChoice == 5) {
            printStatistics();
        } else if (mainMenuChoice == 6) {
            cout << "Understood. Have a nice day." << endl;
            isDone = true;
        } else {
            cout << "Please enter a valid choice." << endl << endl;
        }
    }
}

void SearchEngineUI::clearScreen() {
    for (int i = 0; i < 20; i++) {
        cout << endl;
    }
}

void SearchEngineUI::clearIndex() {
    clearScreen();
    // if the index is already empty notify the user
    if (this->queryProcessor->isIndexEmpty()) {
        cout << "Index is already clear. Press any key then enter to go back to the main menu." << endl;
        string tmp;
        cin >> tmp;
    } else { // otherwise confirm with the user
        bool choiceChosen = false;
        while (choiceChosen == false) {
            char choice;
            cout << "Are you sure you want to clear the index? (y/n)" << endl;
            cin >> choice;
            if (choice == 'y') {
                clearScreen();
                this->queryProcessor->clearIndex();
                cout << "Index cleared!" << endl << endl;
                string pause;
                cout << "Press any key then enter to go back to the main menu." << endl;
                cin >> pause;
                choiceChosen = true;
            } else if (choice == 'n') {
                cout << "Understood. Have a nice day." << endl;
                string pause;
                cout << "Press any key then enter to go back to the main menu." << endl;
                cin >> pause;
                choiceChosen = true;
            } else {
                cout << "Please enter a valid mainMenuChoice." << endl;
            }
        }

    }
}

void SearchEngineUI::parseCorpus() {
    clearScreen();
    bool createIndex = true;
    // if the index isn't empty then warn the user
    if (!this->queryProcessor->isIndexEmpty()) {
        createIndex = false; // dont know if the user wants to create an index or not
        bool choiceChosen = false;
        while (choiceChosen == false) { // in case the user types in an invalid mainMenuChoice
            char choice;
            cout
                    << "The index is not empty. If you continue, the current index will be overwritten. Are you sure? (y/n)"
                    << endl;
            cin >> choice;
            if (choice == 'y') {
                // clear index and give the program the 'thumbs up' to create the index
                this->queryProcessor->clearIndex();
                createIndex = true;
                choiceChosen = true;
            } else if (choice == 'n') {
                // go back to the beginning of the UI
                cout << "Understandable. Have a nice day." << endl;
                choiceChosen = true;
            } else {
                cout << "Please enter a valid (y/n)!" << endl;
            }
        }
    }

    if (createIndex == true) { // create the index
        int noOfKeywords = this->queryProcessor->createIndex();
        printStatistics();
        cout << endl;
        //int cont;
        //cin >> cont;
    }
}

void SearchEngineUI::openPersistenceFile() {
    clearScreen();
    string dir;
    bool openFile = true;
    if (!this->queryProcessor->isIndexEmpty()) { // if index is not empty warn user
        bool isChosen = false;
        char choice;
        // loop is for if the user chooses an invalid choice
        while (isChosen == false) {
            cout << "Current index will be overwriten. Continue? (y/n)";
            cin >> choice;
            cout << endl;
            if (choice == 'y') {
                clearIndex();
                openFile = true;
                isChosen = true;
            } else if (choice == 'n') {
                cout << "Understandable. Have a nice day." << endl;
                openFile = false;
                isChosen = true;
            } else {
                cout << "Please choose a valid choice!" << endl;
            }
        }
    }

    if (openFile == true) {
        int choice;
        bool isChosen = false;

        // good ol' invalid choice loop
        while (isChosen == false) {
            // ask user if they want to use default directory or custom
            cout << "Do you want to use 1) the default directory or 2) a different directory? ";
            cin >> choice;
            cout << endl;
            if (choice == 1) {
                dir = "../persistence";
                isChosen = true;
            } else if (choice == 2) {
                cout << "Enter what directory you would like to use: ";
                string dirPart;
                cin >> dirPart;
                dir = "../" + dirPart;
                isChosen = true;
            } else {
                cout << "Please enter a valid choice!" << endl;
            }
        }

        this->queryProcessor->loadIndices(dir);
        if (!this->queryProcessor->isIndexEmpty()) {
            cout << "File loaded! Here are the stats." << endl;
        }
        printStatistics();
    }
}

void SearchEngineUI::search() {
    string keyword;
    bool brokeOut = false;
    cout << "Enter your search term: ";
    getline(cin, keyword, '\n');
    cout << endl;
    vector<string> *parsedQuery = this->queryProcessor->parseQueryString(keyword);
    set<QueryResultData> searchResults = this->queryProcessor->search(
            parsedQuery[this->queryProcessor->OP][0],
            parsedQuery[this->queryProcessor->KEYWORD],
            parsedQuery[this->queryProcessor->EXCLUSION],
            parsedQuery[this->queryProcessor->AUTHOR]);
    if (searchResults.size() == 0) {
        cout << "Not found." << endl;
        string pause;
        if(!this->queryProcessor->isIndexEmpty()) {
            cout << "Please enter a different search phrase. Type anything then enter to continue." << endl;
        }else{
            cout << "Please type any key then press enter to continue." << endl;
        }
        getline(cin, pause, '\n');
        if(!this->queryProcessor->isIndexEmpty()) {
            search();
        }
    } else {
        while (true) {
            //cout << "Found " << searchResults.size() << " articles." << endl;
            if (searchResults.size() > 15) {
                cout << "Top 15 results: " << endl;
            }
            int i = 0;
            for (auto d : searchResults) {
                i++;
                cout << i << ". " << d.title << endl;
                string publication;
                if (d.publication.length() == 0) {
                    publication = "unknown publication";
                } else {
                    publication = d.publication;
                }
                string authorString = d.authorString;
                int sepPos = authorString.find(",");
                // authors are not sperated by ;, then find the 2 space as the separator of authors
                if (sepPos == string::npos) {
                    sepPos = 0;
                    int spc = 0;
                    for (; sepPos < authorString.length(); sepPos++) {
                        if (authorString[sepPos] == ' ') {
                            spc++;
                        }
                        if (spc == 2) {
                            break;
                        }
                    }
                }
                authorString = authorString[0] == '"' ? authorString.substr(1, sepPos - 1)
                                                      : authorString.substr(0, sepPos - 1);
                if (parsedQuery[this->queryProcessor->AUTHOR].size() > 0) {
                    authorString = parsedQuery[this->queryProcessor->AUTHOR][0];
                    authorString[0] = toupper(authorString[0]);
                }
                authorString += " et al., ";
                cout << publication;

                cout << ". " << authorString;
                cout << d.datePublished << endl << endl;

                if (i == 15) {
                    break;
                }
            }
            cout << endl << "Found " << searchResults.size() << " articles." << endl;
            cout << endl << "Which search result would you like to see the abstract for? (1-15)" << endl;
            cout << "Or do you want to exit to the main menu? (0)" << endl;
            cout << "Or perhaps search again? (16)" << endl;
            int abstractOrQuit;
            string input;
            cout << "Enter your choice here: ";
            cin >> input;
            abstractOrQuit = atoi(input.c_str());
            if (abstractOrQuit >= 1 && abstractOrQuit <= 15) {
                abstractOrQuit--;
                displayAbstract(*(next(searchResults.begin(), abstractOrQuit)));
                cout << "Press q to go back to the main menu or press c to go back to the results" << endl;
                char abstractChoice;
                cin >> abstractChoice;
                if (abstractChoice == 'c') {
                    continue;
                } else {
                    break;
                }
            } else if (abstractOrQuit == 0) {
                break;
            } else { //if (abstractOrQuit == 16) {
                brokeOut = true;
                break;
            }
        }
    }

    if (brokeOut == true) {
        cin.ignore();
        search();
    }

}

void SearchEngineUI::displayAbstract(const QueryResultData &d) {
    clearScreen();
    cout << d.title << endl;
    cout << "Published at " << d.publication;
    cout << ". Published on " << d.datePublished;
    cout << ". Published by " << d.authorString << "." << endl << endl;
    cout << d.abstract << endl << endl;

}

void SearchEngineUI::printStatistics() {
    clearScreen();
    if (this->queryProcessor->isIndexEmpty()) {
        cout
                << "Index is empty. Please select choice 2 or 3 (or 2 if you came here from 3) and then come back. Press any key then enter to go back to the main menu."
                << endl;

        string pause;
        cin >> pause;
    } else {
        cout << "=======================" << endl;
        cout << "=======STATISTICS======" << endl;
        cout << "=======================" << endl << endl;
        cout << "Total articles indexed: " << this->queryProcessor->getTotalArticlesIndexed() << endl;
        cout << "Average number of words indexed per article: " << this->queryProcessor->getAvgWordsIndexedPerArticle()
             << endl;
        cout << "Total words indexed: " << this->queryProcessor->getTotalWordsIndexed() << endl;
        cout << "Total unique authors: " << this->queryProcessor->getTotalUniqueAuthors() << endl;
        cout << "Top 50 most frequent words : " << endl;
        int i = 1;
        for (auto top50OriginalWord: this->queryProcessor->getTop50OriginalWords()) {
            cout << i << ". " << top50OriginalWord;
            if (i % 5 == 0) {
                cout << endl;
            } else {
                cout << ", ";
            }
            i++;
        }
        cout << endl << endl;
        string pause;
        cout << "Press any key then enter to go back.";
        cin >> pause;

    }

}