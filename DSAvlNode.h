//
// Created by Peter Sun on 11/11/20.
//

#ifndef SEARCHENGINE_DSAVLNODE_H
#define SEARCHENGINE_DSAVLNODE_H

#include <iostream>
#include <fstream>
#include <stack>
#include "IndexNodeData.h"
using namespace std;

template<typename T>
class DSAvlNode {
public:
    DSAvlNode<T>(const T &theElement,
                 DSAvlNode<T> *lt,
                 DSAvlNode<T> *rt, int h = 0)
            : element(theElement), left(lt), right(rt), height(h) {}
    DSAvlNode<T> *left;
    DSAvlNode<T> *right;
    int height;
    T element;
};
#endif //SEARCHENGINE_DSAVLNODE_H
