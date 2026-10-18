
// C++ Program to Implement B-Tree
#include <cstdint>
#include <limits>
#include <iostream>
using namespace std;

//Representa null en el archivo binario
constexpr uint64_t DISK_NULL = std::numeric_limits<uint64_t>::max();


// class for the node present in a B-Tree
template <typename T, int ORDER>
class BTreeNode {
public:
    // Array of keys
    T keys[ORDER - 1];
    // Array of child pointers
    uint64_t children[ORDER];
    // Current number of keys
    int n;
    // True if leaf node, false otherwise
    bool leaf;

    BTreeNode(bool isLeaf = true) : n(0), leaf(isLeaf) {
        for (int i = 0; i < ORDER; i++)
            children[i] = DISK_NULL;
    }
};