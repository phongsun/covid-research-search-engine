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

set<string> QueryProcessor::search(string logicOp, vector<string> searchWords, vector<string> excludedWords, vector<string> authors){
    /*set<string> retVal;
    unordered_map<string, unsigned int> documentIdAndIdfMap = this->indexHandler->searchByKeyword(queryString);
    string articleInfo = "";
    for(auto documentIdandIdf: documentIdAndIdfMap){
        string documentId = documentIdandIdf.first;
        ArticleMetaData articleMetaData = indexHandler->metaDataMap[documentId];
        string articleInfo = articleMetaData.publicationDate + " " + articleMetaData.title;
        retVal.insert(articleInfo);
    }
    return retVal;*/


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
        cout<<"exclusions="<<queryString.length()<<"-"<<notPos << endl;
        cout<<exclusionString<<endl;
    }
    if (authorsPos != notPos) {
        authorString = queryString.substr(authorsPos + 7, notPos - authorsPos - 7);
        cout<<"author="<<notPos<<"-"<<(authorsPos+7) << endl;
        cout<< "+==+++==" << authorString<<endl;
    }
    if (op.compare("NONE") != 0 || (notPos == authorsPos )) {
        keywordString = queryString.substr(keywordsPos, authorsPos - keywordsPos);
        cout<<"keywords="<<authorsPos<<"-"<<keywordsPos << endl;
        cout<<keywordString<<endl;
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
    retVal[0] = keyWords;
    retVal[1] = authors;
    retVal[2] = exclusions;
    retVal[3] = logicOp;

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