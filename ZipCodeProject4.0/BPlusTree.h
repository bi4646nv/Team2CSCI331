/**
 * @file BPlusTree.h
 * @brief Definition of the BPlusTree class for managing a B+ Tree index
 */

#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <memory>
#include <string>
#include "IndexBlockBuffer.h"

/**
 * @class BPlusTree
 * @brief Simplified B+ Tree structure for demonstration
 */
class BPlusTree {
private:
    /**
     * @brief Internal structure representing a node in the B+ tree
     */
    struct Node {
        std::shared_ptr<IndexBlockBuffer> buffer; ///< Node buffer (leaf or internal)
        std::vector<std::shared_ptr<Node>> children; ///< Pointers to child nodes (internal nodes only)

        Node(bool isLeaf, int blockSize)
            : buffer(std::make_shared<IndexBlockBuffer>(blockSize, isLeaf)) {}
    };

    std::shared_ptr<Node> root; ///< Root of the B+ Tree
    int blockSize;              ///< Block size in bytes

public:
    /**
     * @brief Constructor
     * @param blockSize Size of each node block
     */
    BPlusTree(int blockSize = 512);

    /**
     * @brief Insert a key and its associated RBN
     * @param key The key to insert
     * @param rbn The associated relative block number
     */
    void insert(const std::string& key, int rbn);

    /**
     * @brief Find the RBN for a given key
     * @param key The key to search
     * @return The corresponding RBN, or -1 if not found
     */
    int find(const std::string& key) const;

    /**
     * @brief Print the structure of the B+ Tree
     */
    void print() const;

private:
    void insertHelper(std::shared_ptr<Node> node, const std::string& key, int rbn);
    int findHelper(std::shared_ptr<Node> node, const std::string& key) const;
    void printHelper(std::shared_ptr<Node> node, int level) const;
};

#endif // BPLUS_TREE_H
