#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include <iostream>
#include "DSAvlTree.h"
#include "DocumentParser.h"

using namespace std;

int main() {
    //Catch::Session().run();
    DocumentParser documentParser;
    documentParser.parse("../corpus");

    return 0;
}
