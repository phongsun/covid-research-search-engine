//
// Created by Peter Sun on 11/17/20.
//

#include "catch.hpp"
#include "DSAvlTree.h"
#include "IndexHandler.h"
#include <stdio.h>

TEST_CASE("DSAvlTree", "[DSAvlTree]"){

    SECTION("Validate Cases 1-4"){
        int dataList[11] = {10, 20, 30, 25, 22, 4, 6, 35, 21, 23, 24};
        DSAvlTree<int> testTree;

        // insert everything from dataList to the AVL tree called testTree
        for (int i = 0; i < 11; i++) {
            testTree.insert(dataList[i]);
        }

        // 22
        REQUIRE(testTree.search(22)->left->element == 20);
        REQUIRE(testTree.search(22)->right->element == 25);
        REQUIRE(testTree.search(22)->height == 3);

        // left of 22
        // 20
        REQUIRE(testTree.search(20)->left->element == 6);
        REQUIRE(testTree.search(20)->right->element == 21);
        REQUIRE(testTree.search(20)->height == 2);
        // 25
        REQUIRE(testTree.search(21)->left == NULL);
        REQUIRE(testTree.search(21)->right == NULL);
        REQUIRE(testTree.search(21)->height == 0);
        // 6
        REQUIRE(testTree.search(6)->left->element == 4);
        REQUIRE(testTree.search(6)->right->element == 10);
        REQUIRE(testTree.search(6)->height == 1);
        // 4
        REQUIRE(testTree.search(4)->left == NULL);
        REQUIRE(testTree.search(4)->right == NULL);
        REQUIRE(testTree.search(4)->height == 0);

        // 10
        REQUIRE(testTree.search(10)->left == NULL);
        REQUIRE(testTree.search(10)->right == NULL);
        REQUIRE(testTree.search(10)->height == 0);

        // right of 22
        // 25
        REQUIRE(testTree.search(25)->left->element == 23);
        REQUIRE(testTree.search(25)->right->element == 30);
        REQUIRE(testTree.search(25)->height == 2);
        // 23
        REQUIRE(testTree.search(23)->left == NULL);
        REQUIRE(testTree.search(23)->right->element == 24);
        REQUIRE(testTree.search(23)->height == 1);
        // 24
        REQUIRE(testTree.search(24)->left == NULL);
        REQUIRE(testTree.search(24)->right == NULL);
        REQUIRE(testTree.search(24)->height == 0);
        // 30
        REQUIRE(testTree.search(30)->left == NULL);
        REQUIRE(testTree.search(30)->right->element == 35);
        REQUIRE(testTree.search(30)->height == 1);
        // 35
        REQUIRE(testTree.search(35)->left == NULL);
        REQUIRE(testTree.search(35)->right == NULL);
        REQUIRE(testTree.search(35)->height == 0);
    }

    SECTION("Tree Deletion") {
        int dataList[11] = {10, 20, 30, 25, 22, 4, 6, 35, 21, 23, 24};
        DSAvlTree<int> *testTree = new DSAvlTree<int>();

        // insert everything from dataList to the AVL tree called testTree
        for (int i = 0; i < 11; i++) {
            testTree->insert(dataList[i]);
        }
        delete testTree;
    }

    SECTION("Tree Iterator") {
        vector<int> d1 = {10, 20, 30, 25, 22, 4, 6, 35, 21, 23, 24};
        DSAvlTree<int> *testTree = new DSAvlTree<int>();

        // insert everything from dataList to the AVL tree called testTree
        for (int i = 0; i < 11; i++) {
            testTree->insert(d1[i]);
        }

        DSAvlTree<int>::Iterator iter = testTree->iterator();
        vector<int> d2;
        while (iter.hasNext()) {
            d2.push_back(iter.next()->element);
        }

        sort(d1.begin(), d1.end());
        sort(d2.begin(), d2.end());
        for (int i = 0; i < 11; i++) {
            REQUIRE(d1[i] == d2[i]);
        }

        delete testTree;
    }

    SECTION( "Serialization" ) {
        IndexHandler *ih = new IndexHandler("../test_data");
        // load files into the index
        ih->createIndex();
        // save the tree to a file
        ih->persistKeywordIndex();

        // read line from the tree file
        string line;
        ifstream myfile;
        myfile.open(ih->getKeyWordIndexFilePath());

        // count lines that are not "NULL"
        REQUIRE(myfile.is_open() == true);
        int cnt = 0;
        while(getline(myfile, line)) {
            if (line.compare(DSAvlTree<IndexNodeData>::INDEX_NODE_DATA_MARKER) != 0) {
                cnt++;
            }
        }
        myfile.close();

        // count of NONE NULL lines needs to equal to the number of nodes in the tree
        REQUIRE(ih->totalWordsIndexed == cnt);
        remove(ih->getKeyWordIndexFilePath().c_str());

        delete ih;
    }

    SECTION( "Deserialization" ) {
        IndexHandler *ih_serialize = new IndexHandler("../test_data");
        // load files into the index
        ih_serialize->createIndex();
        // save serialized tree to the file
        ih_serialize->persistKeywordIndex();

        // restore tree from the file
        IndexHandler *ih_deserialize = new IndexHandler("../test_data");
        REQUIRE(ih_deserialize->isIndexEmpty() == true);
        ih_deserialize->restoreKeywordIndex();

        // retrieved tree should be the same as the saved tree*/
        DSAvlTree<IndexNodeData>::Iterator itS = ih_serialize->getAvlTreeIterator();
        DSAvlTree<IndexNodeData>::Iterator itD = ih_deserialize->getAvlTreeIterator();
        for(int i = 0; i < ih_serialize->totalWordsIndexed; i++){
            REQUIRE(itS.next()->element.equal(itD.next()->element) == true);
        }

        delete ih_deserialize;
        delete ih_serialize;
    }
}