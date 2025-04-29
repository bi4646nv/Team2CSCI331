#include "IndexBlockBuffer.h"
#include <algorithm>
#include <iostream>

/**
 * @brief Constructor for IndexBlockBuffer
 * @param block_size Size of the block in bytes
 * @param is_leaf True if the block is a leaf node in the B+ tree
 */
IndexBlockBuffer::IndexBlockBuffer(int block_size, bool is_leaf)
    : BlockBuffer(block_size), isLeaf(is_leaf) {
    // Empty constructor body; initialization handled above
}

/**
 * @brief Add a key-RBN pair to the index block
 * @param key The key to insert
 * @param rbn The relative block number associated with the key
 * @return Always returns true (placeholder for overflow check)
 */
bool IndexBlockBuffer::addKeyRBNPair(const std::string& key, int rbn) {
    // Add the new pair to the vector
    pairs.push_back(KeyRBNPair(key, rbn));
    
    // Sort the pairs by key to maintain B+ tree order
    std::sort(pairs.begin(), pairs.end(), 
        [](const KeyRBNPair& a, const KeyRBNPair& b) {
            return a.key < b.key;
        });

    return true;
}

/**
 * @brief Find the child RBN associated with a given key
 * @param key The key to search for
 * @return RBN of the child node or -1 if not found
 */
int IndexBlockBuffer::findKey(const std::string& key) const {
    if (pairs.empty()) {
        return -1;  // No entries to search
    }

    if (!isLeaf) {
        // Internal node: Find the child that should be followed
        for (size_t i = 0; i < pairs.size(); i++) {
            if (key <= pairs[i].key) {
                return pairs[i].rbn;
            }
        }
        // If key is greater than all, return the last child pointer
        return pairs.back().rbn;
    }

    // Leaf node: Search for exact match
    for (const auto& pair : pairs) {
        if (pair.key == key) {
            return pair.rbn;
        }
    }

    return -1;  // Not found
}

/**
 * @brief Check if the block is full
 * @return Always returns false (placeholder for actual check)
 */
bool IndexBlockBuffer::isFull() const {
    return false;
}

/**
 * @brief Check if the block has no entries
 * @return true if empty, false otherwise
 */
bool IndexBlockBuffer::isEmpty() const {
    return pairs.empty();
}

/**
 * @brief Set whether this block is a leaf
 * @param leaf True to make it a leaf node, false for internal node
 */
void IndexBlockBuffer::setLeaf(bool leaf) {
    isLeaf = leaf;
}

/**
 * @brief Check whether the block is a leaf node
 * @return true if this is a leaf node
 */
bool IndexBlockBuffer::isLeafNode() const {
    return isLeaf;
}

/**
 * @brief Get the number of key-RBN pairs stored in the block
 * @return The number of pairs
 */
int IndexBlockBuffer::getNumPairs() const {
    return pairs.size();
}

/**
 * @brief Retrieve the key at a specific index
 * @param index Index of the key
 * @return The key if valid index, else an empty string
 */
std::string IndexBlockBuffer::getKeyAt(int index) const {
    if (index < 0 || index >= static_cast<int>(pairs.size())) {
        return "";
    }
    return pairs[index].key;
}

/**
 * @brief Retrieve the RBN at a specific index
 * @param index Index of the RBN
 * @return The RBN if valid index, else -1
 */
int IndexBlockBuffer::getRBNAt(int index) const {
    if (index < 0 || index >= static_cast<int>(pairs.size())) {
        return -1;
    }
    return pairs[index].rbn;
}

/**
 * @brief Print the contents of the index block to console
 */
void IndexBlockBuffer::print() const {
    std::cout << "Index Block: " << (isLeaf ? "Leaf" : "Internal") << " Node" << std::endl;
    std::cout << "Number of Key-RBN Pairs: " << pairs.size() << std::endl;

    for (size_t i = 0; i < pairs.size(); i++) {
        std::cout << "  " << i << ": Key = " << pairs[i].key << ", RBN = " << pairs[i].rbn << std::endl;
    }
}
