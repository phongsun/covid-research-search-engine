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

void IndexNodeData::calculateIdf(unsigned int totalArticlesIndexed) {
    //IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
    int numberOfDocumentsForKeyWord = this->invertedWordFreq.size();
    double ratio = ((double)totalArticlesIndexed)/((double)numberOfDocumentsForKeyWord);
    double result = log(ratio); // increase resolution of the integer result by x 100
    this->idf =  result;
}
