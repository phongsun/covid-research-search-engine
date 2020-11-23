cmake_minimum_required(VERSION 3.15)
project(search-engine-peter-sun)
set (EXE_NAME search_engine_peter_sun)

SET(GCC_COVERAGE_COMPILE_FLAGS "-O3")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set (input01 "stop_words.txt")
set (input02 "test_article.json")
set (input03 "metadata-cs2341.csv")

# Copy data files to build dir
foreach(file IN LISTS input01 input02 input03)
    configure_file(${file} ${file} COPYONLY)
endforeach()

set(CMAKE_CXX_STANDARD 14)

add_executable(${EXE_NAME} simdjson.cpp simdjson.h ArticleData.cpp ArticleData.h main.cpp DSAvlTree.h DSAvlNode.h TestDSAvlTree.cpp DocumentParser.cpp DocumentParser.h porter2_stemmer.cpp porter2_stemmer.h util/hash.h util/string_view.h IndexNodeData.cpp IndexNodeData.h ArticleMetaData.cpp ArticleMetaData.h)