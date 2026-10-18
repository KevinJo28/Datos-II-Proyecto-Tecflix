cmake_minimum_required(VERSION 4.3)
project(api_usuarios)

set(CMAKE_CXX_STANDARD 20)

add_executable(api_usuarios main.cpp
        src/btree/BTreeNode.cpp
        src/btree/BTree.cpp)
