//
// Created by Peter Sun on 11/28/20.
//

#include "QueryResultData.h"
time_t QueryResultData::getPubDate() const {
    char dateArray[this->datePublished.size()];
    // copy datePublished string to the dateArrray
    snprintf(dateArray, sizeof(this->datePublished), "%s", this->datePublished.c_str());

    tm *ltm = new tm; // date time variable

    if(this->datePublished.size() == 4){ // if the date only has year YYYY
        ltm->tm_year = atoi(this->datePublished.c_str());
        ltm->tm_mon = 11;  //get the month value - december is 11
        ltm->tm_mday = 31; //get the day value
        ltm->tm_hour = 1; //get the hour value
        ltm->tm_min = 1; //get the min value
        ltm->tm_sec = 1;
    } else if (this->datePublished.length() == 10){
        char *pch;
        pch = strtok(dateArray, "-");
        ltm->tm_year = atoi(pch); // get year value
        ltm->tm_mon = atoi(strtok(NULL, "-")) - 1; // get month value, 0-11, so minus 1
        ltm->tm_mday = atoi(strtok(NULL, "-")); // get day value
        ltm->tm_hour = 1; //get the hour value
        ltm->tm_min = 1; //get the min value
        ltm->tm_sec = 1;
    }
    return mktime(ltm);
}

bool QueryResultData::operator== (const string& documentId) const{

    return (this->documentId.compare(documentId) == 0);
}

bool QueryResultData::operator== (const QueryResultData& rhs) const{

    if(this->weight == rhs.weight
            && std::difftime(this->getPubDate(), rhs.getPubDate()) == 0 // pub date equals
            && this->title.compare(rhs.title) == 0){
        return true;
    }else{
        return false;
    }
}

bool QueryResultData::operator< (const QueryResultData& rhs) const{
    if (this->weight > rhs.weight){
        return true;
    }else if (this->weight == rhs.weight){
        // compare publicationDOI date
        if (difftime(this->getPubDate(), rhs.getPubDate()) > 0) {
            return true;
        } else if (difftime(this->getPubDate(), rhs.getPubDate()) == 0) { // date same, compare title
            if (this->title.compare(rhs.title) > 0) {
                return true;
            }  else {
                return false;
            }
        } else {
            return false;
        }
    }else{
        return false;
    }
}

bool QueryResultData::operator> (const QueryResultData& rhs) const{
    if (this->weight < rhs.weight){ // inverse the > to < so that set puts highest weight on top
        return true;
    }else if (this->weight == rhs.weight){
    // compare publicationDOI date
        if (difftime(this->getPubDate(), rhs.getPubDate()) < 0) {
            return true;
        } else if (difftime(this->getPubDate(), rhs.getPubDate()) == 0) { // date same, compare title
            if (this->title.compare(rhs.title) < 0) {
                return true;
            }  else {
                return false;
            }
        } else {
            return false;
        }
    } else{
        return false;
    }
}

