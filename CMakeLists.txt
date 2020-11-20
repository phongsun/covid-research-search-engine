cmake_minimum_required(VERSION 3.15)
project(search-engine-peter-sun)
set (EXE_NAME search_engine)

set (input01 "stop_words.txt")
set (input02 "test_article.json")

# Copy data files to build dir
foreach(file IN LISTS input01 input02)
    configure_file(${file} ${file} COPYONLY)
endforeach()

set(CMAKE_CXX_STANDARD 14)

add_executable(${EXE_NAME} simdjson.cpp simdjson.h ArticleData.cpp ArticleData.h main.cpp DSAvlTree.h DSAvlNode.h TestDSAvlTree.cpp DocumentParser.cpp DocumentParser.h porter2_stemmer.cpp porter2_stemmer.h util/hash.h util/string_view.h IndexNodeData.cpp IndexNodeData.h)