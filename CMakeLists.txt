cmake_minimum_required(VERSION 3.15)
project(search_engine_peter_sun)

set(CMAKE_CXX_STANDARD 14)

add_executable(search_engine_peter_sun simdjson.cpp simdjson.h ArticleData.cpp ArticleData.h main.cpp DSAvlTree.h DSAvlNode.h TestDSAvlTree.cpp DocumentParser.cpp DocumentParser.h porter2_stemmer.cpp porter2_stemmer.h util/hash.h util/string_view.h IndexNodeData.cpp IndexNodeData.h)