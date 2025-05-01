/**
 * @file main.cpp
 * @brief Test driver for the B+ Tree with IndexBlockBuffer
 */

#include "BSSManager.h"
#include "BPlusTree.h"
#include <iostream>

int main() {
    // Create a B+ Tree with block size 512 bytes
    BPlusTree bptree(512);

    // Insert some sample key-RBN pairs
    bptree.insert("12345", 1);
    bptree.insert("23456", 2);
    bptree.insert("34567", 3);
    bptree.insert("45678", 4);
    bptree.insert("56789", 5);

    std::cout << "\n=== B+ Tree Structure ===" << std::endl;
    bptree.print();

    // Test searching for keys
    std::string searchKeys[] = { "23456", "00000", "56789", "99999" };

    std::cout << "\n=== Key Search Results ===" << std::endl;
    for (const auto& key : searchKeys) {
        int rbn = bptree.find(key);
        if (rbn != -1) {
            std::cout << "Key " << key << " found with RBN: " << rbn << std::endl;
        } else {
            std::cout << "Key " << key << " not found." << std::endl;
        }
    }

    return 0;
}
<<<<<<< HEAD

=======
>>>>>>> 25984b7 (updated main.cpp)
