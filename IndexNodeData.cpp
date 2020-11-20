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
