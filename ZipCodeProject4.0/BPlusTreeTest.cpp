#include "BPlusTree.h"
#include "ZipCodeRecord.h"
#include <iostream>

int main() {
    BPlusTree tree;

    std::string filename = "zipcode_bplustree.dat";
    std::string datafile = "zipcode.csv";  // Replace with your actual data filename

    // Try to open the tree, or create it if it doesn't exist
    if (!tree.open(filename)) {
        std::cout << "Creating new B+ Tree...\n";
        if (!tree.create(filename, 512, 4)) {
            std::cerr << "Failed to create tree.\n";
            return 1;
        }
    } else {
        std::cout << "Opened existing B+ Tree file.\n";
    }

    // Load data
    std::cout << "Bulk loading records from: " << datafile << "\n";
    if (!tree.bulkLoad(datafile)) {
        std::cerr << "Bulk load failed.\n";
        return 1;
    }

    // Print structure
    tree.print();

    // Dump tree contents to file
    if (!tree.dumpTree("dump.txt")) {
        std::cerr << "Dump failed.\n";
        return 1;
    }

    // Optional: search test
    ZipCodeRecord result;
    std::string searchKey = "56301";
    if (tree.search(searchKey, result)) {
        std::cout << "Search result for key " << searchKey << ": "
                  << result.getPlaceName() << ", " << result.getState() << "\n";
    } else {
        std::cout << "Key " << searchKey << " not found.\n";
    }

    tree.close();
    return 0;
}
