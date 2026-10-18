#include <cstdint>
#include <limits>
#include <iostream>
#pragma once
using namespace std;

//Representa null en el archivo binario
constexpr uint64_t DISK_NULL = std::numeric_limits<uint64_t>::max();

template <typename T, int ORDER>
struct BTreeNode {

    T keys[ORDER - 1];
    uint64_t children[ORDER];
    int n;
    bool leaf;

    BTreeNode(bool isLeaf = true) : n(0), leaf(isLeaf) {
        for (int i = 0; i < ORDER; i++)
            children[i] = DISK_NULL;
    }
};