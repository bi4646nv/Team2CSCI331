#include "BPlusTree.h"
#include <iostream>

/**
 * @brief Constructor initializes the root as a leaf node
 */
BPlusTree::BPlusTree(int blockSize)
    : blockSize(blockSize), root(std::make_shared<Node>(true, blockSize)) {}

/**
 * @brief Insert key and RBN into the tree
 */
void BPlusTree::insert(const std::string& key, int rbn) {
    insertHelper(root, key, rbn);
}

/**
 * @brief Recursive insertion helper (no splitting in this version)
 */
void BPlusTree::insertHelper(std::shared_ptr<Node> node, const std::string& key, int rbn) {
    if (node->buffer->isLeafNode()) {
        node->buffer->addKeyRBNPair(key, rbn);
    } else {
        // Determine the appropriate child
        int childRBN = node->buffer->findKey(key);
        int index = 0;
        for (int i = 0; i < node->buffer->getNumPairs(); ++i) {
            if (key <= node->buffer->getKeyAt(i)) break;
            index = i + 1;
        }

        if (index >= node->children.size()) {
            index = node->children.size() - 1;
        }

        insertHelper(node->children[index], key, rbn);
    }
}

/**
 * @brief Search for a key and return the corresponding RBN
 */
int BPlusTree::find(const std::string& key) const {
    return findHelper(root, key);
}

/**
 * @brief Recursive search helper
 */
int BPlusTree::findHelper(std::shared_ptr<Node> node, const std::string& key) const {
    if (!node) return -1;

    if (node->buffer->isLeafNode()) {
        return node->buffer->findKey(key);
    } else {
        int childRBN = node->buffer->findKey(key);
        int index = 0;
        for (int i = 0; i < node->buffer->getNumPairs(); ++i) {
            if (key <= node->buffer->getKeyAt(i)) break;
            index = i + 1;
        }

        if (index >= node->children.size()) {
            index = node->children.size() - 1;
        }

        return findHelper(node->children[index], key);
    }
}

/**
 * @brief Print the tree structure
 */
void BPlusTree::print() const {
    printHelper(root, 0);
}

/**
 * @brief Recursive tree printing helper
 */
void BPlusTree::printHelper(std::shared_ptr<Node> node, int level) const {
    if (!node) return;

    std::cout << std::string(level * 2, ' ') << "Level " << level << ":" << std::endl;
    node->buffer->print();

    if (!node->buffer->isLeafNode()) {
        for (const auto& child : node->children) {
            printHelper(child, level + 1);
        }
    }
}
