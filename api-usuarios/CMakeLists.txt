cmake_minimum_required(VERSION 4.3)
project(api_usuarios)

set(CMAKE_CXX_STANDARD 20)

add_executable(api_usuarios
        src/btree/BTreeNode.hpp
        src/btree/BTree.hpp
        src/btree/main.cpp)
