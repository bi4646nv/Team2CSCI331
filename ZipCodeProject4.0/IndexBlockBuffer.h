/**
 * @file IndexBlockBuffer.h
 * @brief Definition of the IndexBlockBuffer class for handling B+ tree index blocks
 */

#ifndef INDEX_BLOCK_BUFFER_H
#define INDEX_BLOCK_BUFFER_H

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "BlockBuffer.h"


/**
 * @class IndexBlockBuffer
 * @brief Class for reading and writing index blocks in the B+ tree
 */
class IndexBlockBuffer : public BlockBuffer {
private:
    /**
     * @struct KeyRBNPair
     * @brief Structure to represent a key-RBN pair in the index
     */
    struct KeyRBNPair {
        std::string key;    ///< Zip code
        int rbn;            ///< Relative Block Number pointing to child
        
        /**
         * @brief Constructor
         */
        KeyRBNPair(const std::string& k = "", int r = -1) : key(k), rbn(r) {}
    };
    
    std::vector<KeyRBNPair> pairs;  ///< Vector of key-RBN pairs
    bool isLeaf;                    ///< Indicating if this is a leaf node

public:
    /**
     * @brief Constructor
     * @param block_size Size of a block in bytes
     * @param is_leaf Flag indicating if this is a leaf node
     */
    IndexBlockBuffer(int block_size = 512, bool is_leaf = true)
        : BlockBuffer(block_size), isLeaf(is_leaf) {
        // Initialize as an empty index block
    }


    /**
     * @brief Add a key-RBN pair to the index block
     * @param key The key value
     * @param rbn The RBN value
     * @return true if successful, false if block is full
     */

bool addKeyRBNPair(const std::string& key, int rbn) {
        // Add the pair and sort
        pairs.push_back(KeyRBNPair(key, rbn));
        std::sort(pairs.begin(), pairs.end(), 
            [](const KeyRBNPair& a, const KeyRBNPair& b) {
                return a.key < b.key;
            });
        
        return true;
    }
    
    /**
     * @brief Find the child RBN for a given key
     * @param key The key to search for
     * @return The RBN of the child block, or -1 if not found
     */
    int findKey(const std::string& key) const {
        if (pairs.empty()) {
            return -1;
        }
// For non-leaf nodes, find the appropriate child
        if (!isLeaf) {
            for (size_t i = 0; i < pairs.size(); i++) {
                if (key <= pairs[i].key) {
                    return pairs[i].rbn;
                }
            }
            // If key is greater than all keys, return the last child
            return pairs.back().rbn;
        }
        
        // For leaf nodes, find the exact match
        for (const auto& pair : pairs) {
            if (pair.key == key) {
                return pair.rbn;
            }
        }
        
        return -1;  // If not found
    }
    /**
     * @brief Check if the block is full
     * @return true if the block cannot hold another entry, false otherwise
     */
    bool isFull() const {
        return false;
    }
    
    /**
     * @brief Check if the block is empty
     * @return true if the block has no entries, false otherwise
     */
    bool isEmpty() const {
        return pairs.empty();
    }
    
    /**
     * @brief Set the leaf node flag
     * @param leaf true if this is a leaf node, false otherwise
     */
void setLeaf(bool leaf) {
        isLeaf = leaf;
    }
    
    /**
     * @brief Check if this is a leaf node
     * @return true if this is a leaf node, false otherwise
     */
    bool isLeafNode() const {
        return isLeaf;
    }
    
    /**
     * @brief Get the number of key-RBN pairs
     * @return The number of pairs
     */
    int getNumPairs() const {
        return pairs.size();
    }
/**
     * @brief Get a key at a specific index
     * @param index The index
     * @return The key, or empty string if index is out of range
     */
    std::string getKeyAt(int index) const {
        if (index < 0 || index >= static_cast<int>(pairs.size())) {
            return "";
        }
        return pairs[index].key;
    }
    
    /**
     * @brief Get an RBN at a specific index
     * @param index The index
     * @return The RBN, or -1 if index is out of range
     */
    int getRBNAt(int index) const {
        if (index < 0 || index >= static_cast<int>(pairs.size())) {
            return -1;
        }
        return pairs[index].rbn;
    }
    
    /**
     * @brief Print the contents of the index block
     */
       void print() const {
        std::cout << "Index Block: " << (isLeaf ? "Leaf" : "Internal") << " Node" << std::endl;
        std::cout << "Number of Key-RBN Pairs: " << pairs.size() << std::endl;
        
        for (size_t i = 0; i < pairs.size(); i++) {
            std::cout << "  " << i << ": Key = " << pairs[i].key << ", RBN = " << pairs[i].rbn << std::endl;
        }
    }
};

#endif // INDEX_BLOCK_BUFFER_H
