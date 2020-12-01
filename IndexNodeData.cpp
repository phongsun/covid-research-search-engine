//
// Created by Peter Sun on 11/19/20.
//

#include "IndexNodeData.h"

IndexNodeData::IndexNodeData(){

}

bool IndexNodeData::operator== (const IndexNodeData& rhs) const{
    if(this->keyWord.compare(rhs.keyWord) == 0){
        return true;
    }else{
        return false;
    }
}

bool IndexNodeData::operator< (const IndexNodeData& rhs) const{
    if(this->keyWord.compare(rhs.keyWord) < 0){
        return true;
    }else{
        return false;
    }
}

bool IndexNodeData::operator> (const IndexNodeData& rhs) const{
    if(this->keyWord.compare(rhs.keyWord) > 0){
        return true;
    }else{
        return false;
    }
}

bool IndexNodeData::equal (IndexNodeData& rhs){
    if(this->keyWord.compare(rhs.keyWord) == 0 && this->invertedWordFreq.size() ==  rhs.invertedWordFreq.size()) {
        for (auto k : this->invertedWordFreq) {
            unsigned int rhs_v = rhs.invertedWordFreq[k.first];
            if (k.second != rhs_v) {
                return false;
            }
        }
        return true;

    } else {
        return false;
    }

}

void IndexNodeData::calculateIdf(unsigned int totalArticlesIndexed) {
    //IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
    int numberOfDocumentsForKeyWord = this->invertedWordFreq.size();
    double ratio = ((double)totalArticlesIndexed)/((double)numberOfDocumentsForKeyWord);
    double result = log(ratio); // increase resolution of the integer result by x 100
    this->idf =  result;
}

string IndexNodeData::toJsonString() const {
    // o for object, w for key word
    string jsonString = "{\"o\": {\"w\":\"";
    //add keyword data
    jsonString += keyWord;
    //add tags to wrap keyword
    // c for inverted word count
    jsonString += "\", \"c\": [";

    // loop through the inverted word count by file
    int i = 0;
    int size = this->invertedWordFreq.size();
    for (auto wordCount: this->invertedWordFreq) {
        i++;
        // k for key
        jsonString += "{\"k\":\"";
        jsonString += wordCount.first;
        // v for value
        jsonString += "\",\"v\":\"";
        jsonString += to_string(wordCount.second);
        jsonString += "\"}";
        // if not the last element, add comma to separate array element
        if (i < size) {
            jsonString += ",";
        }
    }
    // wrap it up
    jsonString += "]}}";

    return jsonString;
}