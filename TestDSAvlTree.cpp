//
// Created by Peter Sun on 11/17/20.
//

#include "catch.hpp"
#include "DSAvlTree.h"

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
}