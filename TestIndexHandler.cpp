//
// Created by Peter Sun on 11/28/20.
//

#include "catch.hpp"
#include "IndexHandler.h"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void mem_usage(double& vm_usage, double& resident_set) {
    vm_usage = 0.0;
    resident_set = 0.0;
    ifstream stat_stream("/proc/self/stat",ios_base::in); //get info from proc directory
    //create some variables to get info
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;
    unsigned long vsize;
    long rss;
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
    stat_stream.close();
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configured to use 2MB pages
            vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

TEST_CASE("IndexHandler", "IndexHandler") {
    SECTION("Create Keyword Index") {
        string corpusDir = "../test_data";

        IndexHandler *ih = new IndexHandler(corpusDir);
        ih->createIndex();

        /*cout << "------ " << ih->totalArticlesIndexed << endl;
        cout << "------ " << ih->avgWordsIndexedPerArticle << endl;
        cout << "-----" << ih->totalWordsIndexed << endl;
        cout << "-----" << ih->totalUniqueAuthors << endl;
        int i = 0;
        for (int i = 0; i < 50; i++) {

            cout << ih->top50OriginalWordsData[i].first << " - " << ih->top50OriginalWordsData[i].second << " ^^^";
            cout << ih->topStemmed50WordsData[i].first << " + " << ih->topStemmed50WordsData[i].second << endl;

        }*/
        REQUIRE(ih->totalArticlesIndexed == 100);
        REQUIRE(ih->avgWordsIndexedPerArticle == 594);
        REQUIRE(ih->totalWordsIndexed == 14340);
        REQUIRE(ih->totalUniqueAuthors == 736);
        REQUIRE(ih->top50OriginalWordsData.size() == 50);
        REQUIRE(ih->top50OriginalWordsData[0].second.compare("cells") == 0);
        REQUIRE(ih->top50OriginalWordsData[49].second.compare("including") == 0);
        REQUIRE(ih->topStemmed50WordsData.size() == 50);
        REQUIRE(ih->topStemmed50WordsData[0].second.compare("cell (cells cell ) ") == 0);
        REQUIRE(ih->topStemmed50WordsData[49].second.compare("perform (performed performing performance perform performs performances performer ) ") == 0);

        delete ih;
    }

    SECTION("Clear Keyword Index") {
        SECTION("Clear Index"){

            IndexHandler *ih = new IndexHandler("../test_data");
            ih->createIndex();
            REQUIRE(ih->totalArticlesIndexed == 100);
            REQUIRE(ih->avgWordsIndexedPerArticle == 594);
            REQUIRE(ih->totalWordsIndexed == 14340);
            REQUIRE(ih->totalUniqueAuthors == 736);
            REQUIRE(ih->top50OriginalWordsData.size() == 50);
            REQUIRE(ih->top50OriginalWordsData[0].second.compare("cells") == 0);
            REQUIRE(ih->top50OriginalWordsData[49].second.compare("including") == 0);
            REQUIRE(ih->topStemmed50WordsData.size() == 50);
            REQUIRE(ih->topStemmed50WordsData[0].second.compare("cell (cells cell ) ") == 0);
            REQUIRE(ih->topStemmed50WordsData[49].second.compare("perform (performed performing performance perform performs performances performer ) ") == 0);

            ih->clearIndex();

            REQUIRE(ih->totalArticlesIndexed == 0);
            REQUIRE(ih->avgWordsIndexedPerArticle == 0);
            REQUIRE(ih->totalWordsIndexed == 0);
            REQUIRE(ih->totalUniqueAuthors == 0);
            REQUIRE(ih->top50OriginalWordsData.size() == 0);
            REQUIRE(ih->topStemmed50WordsData.size() == 0);
            delete ih;
            /*double vm2, rss2;
            mem_usage(vm2, rss2);
            cout << "After Delete Index - Virtual Memory: " << vm2 << "\nResident set size: " << rss2 << endl;
*/
        }
    }

    SECTION("Search Author Index") {
        SECTION("Clear Index"){
            IndexHandler *ih = new IndexHandler("../test_data");
            ih->createIndex();

            unordered_set<string> documentIds = ih->searchByAuthor("wang");
            REQUIRE(documentIds.size() == 14);
            delete ih;
        }
    }
}