#include <iostream>
#include "DSAvlTree.h"

using namespace std;

int main() {
    int dataList[11] = {10, 20, 30, 25, 22, 4, 6, 35, 21, 23, 24};
    DSAvlTree<int> testTree;
    for (int i = 0; i < 11; i++) {
        testTree.insert(dataList[i]);
    }

    DSAvlNode<int> *node = testTree.search(25);
    cout << "node height = " << node->height << endl;

    return 0;
}
