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

inline void QueryProcessor::searchKeywordIndex(const vector<string> &searchWords,
                                                vector<IndexNodeData*> &searchResults,
                                                vector<set<string>> &documentIDSets) {

    for(int i = 0; i < searchWords.size(); i++){ // go to the index to search every search word
        IndexNodeData* searchResult;
        string searchPhrase = searchWords[i];
        preprocess(searchPhrase);
        searchResult = this->indexHandler->searchByKeyword(searchPhrase);
        if(searchResult != nullptr) {
            searchResults.push_back(searchResult);
        }
    }

    // create a set of document IDs for each result
    for (auto searchResult: searchResults) {
        set<string> documentIDSet;
        for (auto docIDAndTP: searchResult->invertedTermFreq) {
            documentIDSet.insert(docIDAndTP.first);
        }
        documentIDSets.push_back(documentIDSet);
    }
}

inline void QueryProcessor::buildQueryResult(const vector<IndexNodeData*> &searchResults,
                             const set<string> &intersectionList,
                             const set<string> &unionList,
                             const set<string> &authors,
                             const set<string> &excludedWords,
                             set<QueryResultData> &queryResultSet){

    // filter the searchResults array based on the union of the document ID
    for(auto searchResult: searchResults) {
        // loop through each search result
        for (auto docIdAndTf: searchResult->invertedTermFreq) {
            // determine if the current document ID is in the intersection
            if ((intersectionList.size() > 0 && (intersectionList.find(docIdAndTf.first) != intersectionList.end()))
                ||
                (unionList.size() > 0 && (unionList.find(docIdAndTf.first) != unionList.end()))
                ||
                (excludedWords.size() > 0 && (excludedWords.find(docIdAndTf.first) == excludedWords.end()))
                ||
                (intersectionList.size() == 0 && unionList.size() == 0) // single keyword search
                )
                { // only add the search result to the query result data if the documentID is in the documentIDSets
                QueryResultData queryResultData;
                queryResultData.documentId = docIdAndTf.first;
                queryResultData.tf = docIdAndTf.second;
                queryResultData.idf = searchResult->idf;
                queryResultData.weight = queryResultData.tf * queryResultData.idf;
                queryResultData.publicationDate = this->indexHandler->metaDataMap[docIdAndTf.first].publicationDate;
                queryResultData.title = this->indexHandler->metaDataMap[docIdAndTf.first].title;
                // abstract and authors
                queryResultData.authorString = this->indexHandler->metaDataMap[docIdAndTf.first].author;
                queryResultData.abstract = this->indexHandler->metaDataMap[docIdAndTf.first].abstract;
                bool isInsert = true;
                // go through existing result set
                for(auto d : queryResultSet) {
                    // if the document id already exist in the result set
                    if (d ==  queryResultData.documentId) {
                        // check if the weight of existing element is < than the new weight
                        if (d.weight < queryResultData.weight) {
                            // assign th bigger weight to exiting element.
                            d.weight = queryResultData.weight;
                        }
                        isInsert = false;
                        break;
                    }
                }
                if (isInsert)
                    queryResultSet.insert(queryResultData);
            }
        }
    }
}

set<QueryResultData> QueryProcessor::search(string logicOp, vector<string> searchWords, vector<string> excludedWords, vector<string> authors){
    set<QueryResultData> queryResultSet;
    bool shouldExclude;
    if(searchWords.size() == 0 && authors.size() == 0){
        shouldExclude = false;
    }

    // set of exclusion document IDs
    set<string> exclusionResult;
    if(excludedWords.size() > 0){

    }

    // declare an array of search results corresponding to the keywords
    vector<IndexNodeData*> searchResults;
    // set of the document IDs for each search result
    vector<set<string>> documentIDSets;
    // set that stores the intersectionResult of all of the documentIDSets
    set<string> unionResult;
    // set that stores the intersectionResult of all of the documentIDSets
    set<string> intersectionResult;
    // set that stores the list of documentIDs for author search
    set<string> authorResult;

    if(searchWords.size() > 0) {
        this->searchKeywordIndex(searchWords, searchResults, documentIDSets);
    }

    // if the logic operator is AND
    if(logicOp.compare("AND") == 0){

        if (searchResults.size() == 0) {
            return queryResultSet;
        }

        vector<set<string>> tmpIntersectionList;
        // intersect the documentID from the searchResults array
        if(documentIDSets.size() > 1) { //multple search results
            set<string> tmp0;
            set_intersection(documentIDSets[0].begin(), documentIDSets[0].end(), documentIDSets[1].begin(), documentIDSets[1].end(), inserter(tmp0, tmp0.begin()));
            tmpIntersectionList.push_back(tmp0);
            for(int i = 2; i < documentIDSets.size(); i++){
                set<string> prevIntersection = *(tmpIntersectionList.end() - 1);//last element of the vector
                set<string> curIntersection;
                set_intersection(documentIDSets[i].begin(), documentIDSets[i].end(), prevIntersection.begin(), prevIntersection.end(), inserter(curIntersection, curIntersection.begin()));
                tmpIntersectionList.push_back(curIntersection);
            }
            intersectionResult = *(tmpIntersectionList.end() - 1);
        } else { // only one search result
            intersectionResult = documentIDSets[0];
        }


    }

    else if(logicOp.compare("OR") == 0){


        if (searchResults.size() == 0) { // if no results found
            return queryResultSet; // return empty result set
        }

        // union the documentID from the searchResults array
        if(documentIDSets.size() > 1) {
            set_union(documentIDSets[0].begin(), documentIDSets[0].end(), documentIDSets[1].begin(), documentIDSets[1].end(), inserter(unionResult, unionResult.begin()));
            cout << "*****" << documentIDSets[0].size() << " + " << documentIDSets[1].size() << " = " << unionResult.size() << endl;
        } else {
            unionResult = documentIDSets[0];
        }
        /*for(int i = 2; i < documentIDSets.size(); i++){
            set_intersection(documentIDSets[i].begin(), documentIDSets[i].end(), documentIDSets.begin(), documentIDSets.end(), inserter(intersectionResult, intersectionResult.begin()));
        }*/
    }

    else if(logicOp.compare("NONE") == 0 && excludedWords.size() == 0 && authors.size() == 0){ // single keyword scenario
        // declare an array of search results corresponding to the keywords
        vector<IndexNodeData*> searchResults;
        // set of the document IDs for each search result
        vector<set<string>> documentIDSets;

        this->searchKeywordIndex(searchWords, searchResults, documentIDSets);

        if (searchResults.size() == 0) { // if no results found
            return queryResultSet; // return empty result set
        }


    }


    buildQueryResult(searchResults,
                     intersectionResult,
                     unionResult,
                     authorResult,
                     exclusionResult,
                     queryResultSet);
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
    }else {
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