//
// Created by Peter Sun on 11/28/20.
//

#include "QueryResultData.h"
time_t QueryResultData::getPubDate() const {
    char dateArray[this->publicationDate.size()];
    // copy publicationDate string to the dateArrray
    snprintf(dateArray, sizeof(this->publicationDate), "%s", this->publicationDate.c_str());

    tm *ltm = new tm; // date time variable

    if(this->publicationDate.size() == 4){ // if the date only has year YYYY
        ltm->tm_year = atoi(this->publicationDate.c_str());
        ltm->tm_mon = 11;  //get the month value - december is 11
        ltm->tm_mday = 31; //get the day value
        ltm->tm_hour = 1; //get the hour value
        ltm->tm_min = 1; //get the min value
        ltm->tm_sec = 1;
    } else if (this->publicationDate.length() == 10){
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

bool QueryResultData::operator== (const QueryResultData& rhs) const{
    if(this->tf == rhs.tf && this->idf == rhs.idf
            && std::difftime(this->getPubDate(), rhs.getPubDate()) == 0 // pub date equals
            && this->title.compare(rhs.title) == 0){
        return true;
    }else{
        return false;
    }
}

bool QueryResultData::operator< (const QueryResultData& rhs) const{
    if (this->tf > rhs.tf){
        return true;
    }else if (this->tf == rhs.tf){
        if(this->idf > rhs.idf){
            return true;
        } else if (this->idf == rhs.idf){  // compare publication date
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
        } else {
            return false;
        }
    }else{
        return false;
    }
}

bool QueryResultData::operator> (const QueryResultData& rhs) const{
    if (this->tf < rhs.tf){
        return true;
    }else if (this->tf == rhs.tf){
        if(this->idf < rhs.idf){
            return true;
        } else if (this->idf == rhs.idf){  // compare publication date
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
        } else {
            return false;
        }
    }else{
        return false;
    }
}

