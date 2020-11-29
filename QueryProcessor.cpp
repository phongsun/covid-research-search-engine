//
// Created by Peter Sun on 11/27/20.
//

#include "QueryProcessor.h"

QueryProcessor::QueryProcessor(IndexHandler *indexHandler){
    this->indexHandler = indexHandler;
}

unsigned int QueryProcessor::createIndex(){
    return this->indexHandler->createIndex();
}

set<QueryResultData> QueryProcessor::search(string logicOp, vector<string> searchWords, vector<string> excludedWords, vector<string> authors){
    set<QueryResultData> queryResultSet;

    // if the logic operator is AND
    if(logicOp.compare("AND") == 0){
        // declare an array of search results corresponding to the keywords
        vector<IndexNodeData*> searchResults;
        // set of the document IDs for each search result
        vector<set<string>> documentIDSets;
        // set that stores the intersection of all of the documentIDSets
        set<string> intersection;
        for(int i = 0; i < searchWords.size(); i++){ // go to the index to search every search word
            IndexNodeData* searchResult;
            searchResult = this->indexHandler->searchByKeyword(searchWords[i]);
            if(searchResult != nullptr) {
                searchResults.push_back(searchResult);
            }
        }

        if (searchResults.size() == 0) {
            return queryResultSet;
        }

        for (auto searchResult: searchResults) {
            set<string> documentIDSet;
            for (auto docIDAndTP: searchResult->invertedTermFreq) { // create a set of document IDs for each result to prepare to intersect;
                documentIDSet.insert(docIDAndTP.first);
            }
            documentIDSets.push_back(documentIDSet);
        }

        // union the documentID from the searchResults array
        if(documentIDSets.size() > 1) {
            set_intersection(documentIDSets[0].begin(), documentIDSets[0].end(), documentIDSets[1].begin(), documentIDSets[1].end(), inserter(intersection, intersection.begin()));
        } else {
            intersection = documentIDSets[0];
        }
        /*for(int i = 2; i < documentIDSets.size(); i++){
            set_intersection(documentIDSets[i].begin(), documentIDSets[i].end(), documentIDSets.begin(), documentIDSets.end(), inserter(intersection, intersection.begin()));
        }*/

        // filter the searchResults array based on the union of the document ID
        for(auto searchResult: searchResults) {
            // loop through each search result
            for (auto docIdAndTf: searchResult->invertedTermFreq) {
                // determine if the current document ID is in the intersection
                if(intersection.find(docIdAndTf.first) != intersection.end()) { // only add the search result to the query result data if the documentID is in the documentIDSets
                    QueryResultData queryResultData;
                    queryResultData.documentId = docIdAndTf.first;
                    queryResultData.tf = docIdAndTf.second;
                    queryResultData.idf = searchResult->inverseDocFreq;
                    queryResultData.publicationDate = this->indexHandler->metaDataMap[docIdAndTf.first].publicationDate;
                    queryResultData.title = this->indexHandler->metaDataMap[docIdAndTf.first].title;
                    // abstract and authors
                    queryResultData.authorString = this->indexHandler->metaDataMap[docIdAndTf.first].author;
                    queryResultData.abstract = this->indexHandler->metaDataMap[docIdAndTf.first].abstract;
                    queryResultSet.insert(queryResultData);
                }
            }
        }
    }

    else if(logicOp.compare("NONE") == 0 && excludedWords.size() == 0 && authors.size() == 0){ // single keyword scenario
        string singleKeyword = searchWords[0];
        preprocess(singleKeyword);
        // search index
        IndexNodeData* singleKeywordSearchResults = this->indexHandler->searchByKeyword(singleKeyword);

        // combine QueryResultData
        for(auto docIdAndTf: singleKeywordSearchResults->invertedTermFreq){
            QueryResultData queryResultData;
            queryResultData.documentId = docIdAndTf.first;
            queryResultData.tf = docIdAndTf.second;
            queryResultData.idf = singleKeywordSearchResults->inverseDocFreq;
            queryResultData.publicationDate = this->indexHandler->metaDataMap[docIdAndTf.first].publicationDate;
            queryResultData.title = this->indexHandler->metaDataMap[docIdAndTf.first].title;
            // abstract and authors
            queryResultData.authorString = this->indexHandler->metaDataMap[docIdAndTf.first].author;
            queryResultData.abstract = this->indexHandler->metaDataMap[docIdAndTf.first].abstract;
            queryResultSet.insert(queryResultData);
        }
    }


    return queryResultSet;
}

vector<string>* QueryProcessor::parseQueryString(const string &queryString){
    vector<string> *retVal = new vector<string>[4];

    string authorString;
    string keywordString;
    string exclusionString;

    vector<string> logicOp;
    vector<string> keyWords;
    vector<string> authors;
    vector<string> exclusions;

    int keywordsPos = -1;
    int authorsPos = queryString.find("AUTHOR ");
    int notPos = queryString.find(" NOT ");

    string op;
    if (queryString.find("AND ") == 0) {
        op = "AND";
        keywordsPos = 4;
    } else if (queryString.find("OR ") == 0) {
        op = "OR";
        keywordsPos = 3;
    } else {
        op = "NONE";
        keywordsPos = 0;
    }
    logicOp.push_back(op);
    notPos = notPos == string::npos ? queryString.length() : notPos;
    authorsPos = authorsPos == string::npos ? notPos : authorsPos;

    if (notPos < queryString.length()) {
        exclusionString = queryString.substr(notPos + 5, queryString.length() - notPos);
        //cout<<"exclusions="<<queryString.length()<<"-"<<notPos << endl;
        //cout<<exclusionString<<endl;
    }
    if (authorsPos != notPos) {
        authorString = queryString.substr(authorsPos + 7, notPos - authorsPos - 7);
        //cout<<"author="<<notPos<<"-"<<(authorsPos+7) << endl;
        //cout<< "+==+++==" << authorString<<endl;
    }
    if (op.compare("NONE") != 0 || (notPos == authorsPos )) {
        keywordString = queryString.substr(keywordsPos, authorsPos - keywordsPos);
        //cout<<"keywords="<<authorsPos<<"-"<<keywordsPos << endl;
        //cout<<keywordString<<endl;
    }

    if (!keywordString.empty()) {
        istringstream iss(keywordString);
        keyWords = {istream_iterator<string>{iss}, istream_iterator<string>{}};
    }
    if (!authorString.empty()) {
        istringstream aus(authorString);
        //cout << "*****" << authorString << endl;
        authors = {istream_iterator<string>{aus}, istream_iterator<string>{}};
        //cout << "*****" << authors[0] << endl;
    }
    if (!exclusionString.empty()) {
        istringstream nos(exclusionString);
        exclusions = {istream_iterator<string>{nos}, istream_iterator<string>{}};
    }
    retVal[KEYWORD] = keyWords;
    retVal[AUTHOR] = authors;
    retVal[EXCLUSION] = exclusions;
    retVal[OP] = logicOp;

    return retVal;
}

void QueryProcessor::preprocess(string &input, bool shouldStem) {
    // 1. get rid of space, numbers, and special characters
    input.erase(std::remove_if(input.begin(), input.end(),
            []( auto const& c ) -> bool
            { return !std::isalpha(c); } ), input.end());
    // 2. convert to lower case
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    // 3. stem
    if(shouldStem == true){
        Porter2Stemmer::stem(input);
    }
}