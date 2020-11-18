cmake_minimum_required(VERSION 3.15)
project(search_engine_peter_sun)

set(CMAKE_CXX_STANDARD 14)

add_executable(search_engine_peter_sun simdjson.cpp simdjson.h main.cpp DSAvlTree.h DSAvlNode.h TestDSAvlTree.cpp DocumentParser.cpp DocumentParser.h)