//
// Created by Peter Sun on 11/11/20.
//

#ifndef SEARCHENGINE_DSAVLTREE_H
#define SEARCHENGINE_DSAVLTREE_H

#include "DSAvlNode.h"

template<typename T>
class DSAvlTree{
public:
    DSAvlTree() {
        this->root = nullptr;
    }
    void insert(const T &x) {
        this->insert(x, this->root);
    }

    DSAvlNode<T> *search(T data) {
        return search(data, this->root);
    }

private:
    DSAvlNode<T> *root;

    void insert(const T &x, DSAvlNode<T> * &t);
    void rotateWithLeftChild(DSAvlNode<T>* &k2);
    void rotateWithRightChild(DSAvlNode<T>* &k1);
    void doubleWithRightChild(DSAvlNode<T>* &k3);
    void doubleWithLeftChild(DSAvlNode<T>* &k3);
    DSAvlNode<T> *search(T data, DSAvlNode<T> * &c);

    void preOrder(DSAvlNode<T> *c) {
        if (c!= nullptr) {
            cout << c->element << endl;
            preOrder(c->left);
            preOrder(c->right);
        }
    }

    void postOrder(DSAvlNode<T> *c) {
        if (c!= nullptr) {
            postOrder(c->left);
            postOrder(c->right);
            cout << c->element<< endl;
        }
    }

    void inOrder(DSAvlNode<T> *c) {
        if (c!= nullptr) {
            inOrder(c->left);
            cout << c->element << endl;
            inOrder(c->right);
        }
    }

    int height(DSAvlNode<T> *t) const {
        return t == nullptr ? -1 : t->height;
    }

    int max(int lhs, int rhs) const {
        return lhs > rhs ? lhs : rhs;
    }
};

template<typename T>
DSAvlNode<T>* DSAvlTree<T>::search(T data, DSAvlNode<T> * &c){
    if(c!= nullptr)
    {
        if(data == c->element)
            return c;
        if(data < c->element)
            return search(data, c->left);
        else
            return search(data, c->right);
    }
    else {
        return nullptr;
    }
}

template<typename T>
void DSAvlTree<T>::insert(const T &x, DSAvlNode<T>* &t){
    if (t == nullptr) {
        t = new DSAvlNode<T>(x, nullptr, nullptr);

    } else if (x < t->element) {
        insert(x, t->left);

        if (height(t->left) - height(t->right) == 2) {
            if (x < t->left->element) { // case 1
                rotateWithLeftChild(t);

            } else { // case 2
                doubleWithLeftChild(t);
            }
        }
    } else if (x > t->element) {
        insert(x, t->right);

        if ( height(t->right) - height(t->left) == 2) {
            if (x > t->right->element) { // case 4
                rotateWithRightChild(t);

            } else { // case 3
                doubleWithRightChild(t);
            }
        }
    } else ;

    t->height = max(height(t->left), height(t->right)) + 1;
}

template<typename T>
void DSAvlTree<T>::rotateWithLeftChild(DSAvlNode<T>* &k2) {
    DSAvlNode<T>* k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), k2->height) + 1;
    k2 = k1;
}

template<typename T>
void DSAvlTree<T>::rotateWithRightChild(DSAvlNode<T>* &k1) {
    DSAvlNode<T>* k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    k1->height = max(height(k1->left), height(k1->right)) + 1;
    k2->height = max(height(k2->right), k1->height) + 1;
    k1 = k2;
}

template<typename T>
void DSAvlTree<T>::doubleWithLeftChild(DSAvlNode<T>* &k3) {
    rotateWithRightChild(k3->left);
    rotateWithLeftChild(k3);
}

template<typename T>
void DSAvlTree<T>::doubleWithRightChild(DSAvlNode<T>* &k1) {
    rotateWithLeftChild(k1->right);
    rotateWithRightChild(k1);
}
#endif //SEARCHENGINE_DSAVLTREE_H
