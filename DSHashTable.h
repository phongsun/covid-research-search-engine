//
// Created by Peter Sun on 11/16/20.
//

#ifndef SEARCHENGINE_DSHASHTABLE_H
#define SEARCHENGINE_DSHASHTABLE_H
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <functional>
#include "MurmurHash3.h"

using namespace std;

template<typename K, typename V>
class DSHashTable {
private:
    typedef typename std::list<pair<K,V>>::iterator BucketListIterator;

public:
    class Iterator { // beginning of Iterator Class
    public:
        friend class DSHashTable<K, V>; // allows access to private variables

    public:
        Iterator() : hashTable(0), currentIndex(-1) {}

        Iterator(Iterator const &iter)
                : hashTable(iter.hashTable), currentIndex(iter.currentIndex), currentBucketIter(iter.currentBucketIter) {}

        Iterator &operator=(const Iterator &old) {
            hashTable = old.hashTable;
            currentIndex = old.currentIndex;
            currentBucketIter = old.currentBucketIter;
            return *this;
        }

        // The dereference operator need only worry about the current
        // bucket iterator, and does not need to check the current index.
        const pair<K, V> &operator*() const { return *currentBucketIter; }

        // The comparison operators must account for the bucket iterators
        // being unassigned at the end.
        friend bool operator==(const Iterator &left, const Iterator &right) {
            return left.hashTable == right.hashTable && left.currentIndex == right.currentIndex &&
                   (left.currentIndex == -1 || left.currentBucketIter == right.currentBucketIter);
        }

        friend bool operator!=(const Iterator &left, const Iterator &right) {
            return left.hashTable != right.hashTable || left.currentIndex != right.currentIndex ||
                   (left.currentIndex != -1 && left.currentBucketIter != right.currentBucketIter);
        }

        Iterator &operator++() {
            this->next();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp(*this);
            this->next();
            return temp;
        }

        Iterator &operator--() {
            this->prev();
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp(*this);
            this->prev();
            return temp;
        }

    private:
        DSHashTable<K, V> *hashTable;
        int currentIndex; // current bucket/index in the hash table
        BucketListIterator currentBucketIter; // current bucket iterator at the current bucket/index

        // private constructors for use by the DSHashtable only
        Iterator(DSHashTable<K, V> *ht) : hashTable(ht), currentIndex(-1) {}

        Iterator(DSHashTable<K, V> *ht, int index, BucketListIterator loc)
                : hashTable(ht), currentIndex(index), currentBucketIter(loc) {}

        // Find the next entry in the hashtable
        void next() {
            // next item in the bucket
            ++currentBucketIter;
            // If we are at the end of this bucket
            if (currentBucketIter == hashTable->table[currentIndex].end()) {
                // Find the next non-empty bucket in the table
                for (++currentIndex;
                     currentIndex < int(hashTable->table.size()) && hashTable->table[currentIndex].empty();
                     ++currentIndex) {}

                // If one is found, assign the currentBucketIter to the start
                if (currentIndex != int(hashTable->table.size())) {
                    currentBucketIter = hashTable->table[currentIndex].begin();

                } else {// Otherwise, we are at the end of the hashtable
                    currentIndex = -1;
                }
            }
        }

        // Find the previous entry in the table
        void prev() {
            // If we aren’t at the start of the current bucket, just decrement
            // the current buckety iterator
            if (currentBucketIter != hashTable->table[currentIndex].begin())
                currentBucketIter--;
            else {
                // Otherwise, back down the table until the previous
                // non-empty list in the table is found
                for (--currentIndex; currentIndex >= 0 && hashTable->table[currentIndex].empty(); --currentIndex) {}

                // Go to the last entry in the bucket.
                currentBucketIter = hashTable->table[currentIndex].begin();
                BucketListIterator p = currentBucketIter;
                ++p;
                for (; p != hashTable->table[currentIndex].end(); ++p, ++currentBucketIter) {}
            }
        }
    }; // end of ITERATOR CLASS

private:
    vector<list<pair<K, V>>> table;
    unsigned int tableSize;

    // resize the table with the same values but a
    void resize_table(uint32_t newSize) {
        this->table.resize(newSize);
        this->tableSize = newSize;
    }
    inline unsigned int hash(const K &s)
    {
        uint32_t seed = 1;
        uint32_t hashValue[1];  // allocate 32 bits

        MurmurHash3_x86_32((const void *) s.c_str(), s.length(), seed, hashValue);

        return hashValue[0];
    }

public:
    DSHashTable<K, V>(unsigned int initSize = 100000) : table(initSize), tableSize(0) {}

    DSHashTable<K, V>(const DSHashTable<K, V> &old) : table(old.table), tableSize(old.tableSize) {}

    ~DSHashTable<K, V>() { table.clear(); }

    DSHashTable<K, V> &operator=(const DSHashTable<K, V> &old) {
        if (&old != this)
            *this = old;
    }

    unsigned int size() const {
        return tableSize;
    }

    /*int count() const {
        int cnt = 0;
        Iterator p(this);
        while(*p != table.empty()) {
            cnt++;
            p++;
        }

        return cnt;
    }*/

    const V& operator[] (const K& k)  {
        Iterator iter = find(k);
        return (*iter).second;
    }

    // Insert the key if it is not already there.
    std::pair<Iterator, bool> insert(const pair<K, V> &data) {
        const float LOAD_FRACTION_FOR_RESIZE = 1.25;
        if (tableSize >= LOAD_FRACTION_FOR_RESIZE * table.size())
            this->resize_table(2 * table.size() + 1);

        list<pair<K, V>> listForTheBucket;
        //  get the hash and get the bucket/index
        unsigned hashValue = hash(data.first);
        unsigned int newIdx = hashValue % table.size();

        // if bucket does have value, then get bucket iterator
        if (table[newIdx].begin() != table[newIdx].end()) {
            listForTheBucket = table[newIdx];
            for (BucketListIterator bt = listForTheBucket.begin(); bt != listForTheBucket.end(); bt++) {
                //cout << "conflict for " << data.first << ": " << (*bt).first << endl;
                if ((*bt).first.compare(data.first) == 0) {
                    return pair<Iterator, bool>(Iterator(this), false);
                }

            }
        }

        listForTheBucket.push_back(data);
        table[newIdx] = listForTheBucket;
        // iterator point to the bucket and bucket iterator which points to the newly push_back element
        Iterator p(this, newIdx, --listForTheBucket.end());
        return pair<Iterator, bool>(p, true);
    }

    // Find the key, using hash function, indexing and list find
    Iterator find(const K &key) {
        unsigned int hashValue = this->hash(key);
        unsigned int index = hashValue % table.size();
        BucketListIterator p = table[index].begin();
        for (; p != table[index].end(); p++) {
            if ((*p).first.compare(key) == 0) {
                break;
            }
        }

        if (p == table[index].end())
            return this->end();
        else
            return Iterator(this, index, p);
    }

    // Erase the key
    bool erase(const K &key) {
        // Find the key and use the erase iterator function.
        Iterator p = find(key);
        if (p == end())
            return false;
        else {
            erase(p);
            return true;
        }
    }

    // Erase at the iterator
    void erase(Iterator p) {
        table[p.currentIndex].erase(p.currentBucketIter);
    }

    // Find the first entry in the table and create an associated iterator
    Iterator begin() {
        Iterator p(this);
        p.currentIndex = 0;
        return p;
    }

    // Create an end iterator.
    Iterator end() {
        Iterator p(this);
        p.currentIndex = -1;
        return p;
    }

    // A public print utility.
    void print(std::ostream &ostr) {
        for (unsigned int i = 0; i < table.size(); ++i) {

            ostr << i << ": ";
            for (BucketListIterator p = table[i].begin(); p != table[i].end(); ++p)
                ostr << ' ' << *p;
            ostr << std::endl;
        }
    }

    // A public print utility.
    int count() {
        int cnt = 0;
        for (unsigned int i = 0; i < table.size(); ++i) {
            for (BucketListIterator p = table[i].begin(); p != table[i].end(); ++p)
                cnt++;
        }

        return cnt;
    }

    void erase() {
        table.clear();
    }
};

#endif //SEARCHENGINE_DSHASHTABLE_H
