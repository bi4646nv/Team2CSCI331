#include "BPlusTree.h"
#include <iostream>
#include <fstream>
#include <iomanip>  // for std::setw

/**
 * @brief Constructor initializes header values.
 */
BPlusTree::BPlusTree() {
    header.rootRBN = -1;
    header.height = 0;
    header.totalBlocks = 0;
    header.firstLeafRBN = -1;
    header.lastLeafRBN = -1;
    header.blockSize = 0;
    header.order = 0;
    header.headerSize = sizeof(HeaderRecord);
}

/**
 * @brief Destructor closes the file.
 */
BPlusTree::~BPlusTree() {
    close();
}

/**
 * @brief Create a new B+ tree file.
 * Initializes and writes the header.
 * 
 * @param fname Name of the file to create
 * @param bSize Size of each block in bytes
 * @param treeOrder Order of the B+ tree
 * @return true if successful, false otherwise
 */
bool BPlusTree::create(const std::string& fname, int bSize, int treeOrder) {
    filename = fname;
    blockSize = bSize;
    order = treeOrder;

    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    header.blockSize = blockSize;
    header.order = order;
    header.headerSize = sizeof(HeaderRecord);
    header.rootRBN = 0;
    header.totalBlocks = 1;
    header.height = 1;

    file.close();
    return writeHeader();
}

/**
 * @brief Open an existing B+ tree file.
 * 
 * @param fname Name of the file to open
 * @return true if successful, false otherwise
 */
bool BPlusTree::open(const std::string& fname) {
    filename = fname;
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    return readHeader();
}

/**
 * @brief Close the B+ tree file if open.
 */
void BPlusTree::close() {
    if (file.is_open()) file.close();
}

/**
 * @brief Writes the header record to the beginning of the file.
 * 
 * @return true if successful, false otherwise
 */
bool BPlusTree::writeHeader() {
    std::ofstream out(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!out) return false;

    out.seekp(0, std::ios::beg);
    out.write(reinterpret_cast<char*>(&header), sizeof(HeaderRecord));
    out.close();
    return true;
}

/**
 * @brief Reads the header record from the file.
 * 
 * @return true if successful, false otherwise
 */
bool BPlusTree::readHeader() {
    if (!file.is_open()) return false;

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&header), sizeof(HeaderRecord));
    return true;
}

/**
 * @brief Print the B+ Tree structure and header information to the console.
 */
void BPlusTree::print() const {
    std::cout << "---- B+ Tree Structure ----\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Block size: " << header.blockSize << " bytes\n";
    std::cout << "Tree height: " << header.height << "\n";
    std::cout << "Total blocks: " << header.totalBlocks << "\n";
    std::cout << "Root RBN: " << header.rootRBN << "\n";
    std::cout << "First leaf RBN: " << header.firstLeafRBN << "\n";
    std::cout << "Last leaf RBN: " << header.lastLeafRBN << "\n";
    std::cout << "----------------------------\n";
}

/**
 * @brief Dump the contents of all blocks in the B+ Tree file to a text file.
 * Used for debugging and testing to verify correct structure.
 * 
 * @param outputFile The name of the output text file
 * @return true if dump was successful, false otherwise
 */
bool BPlusTree::dumpTree(const std::string& outputFile) const {
    std::ofstream out(outputFile);
    if (!out) {
        std::cerr << "Failed to open output file: " << outputFile << "\n";
        return false;
    }

    out << "---- Dumping B+ Tree Blocks ----\n";
    out << "Total blocks: " << header.totalBlocks << "\n\n";

    for (int rbn = 0; rbn < header.totalBlocks; ++rbn) {
        file.seekg(header.headerSize + rbn * header.blockSize, std::ios::beg);
        std::vector<char> buffer(header.blockSize);
        file.read(buffer.data(), header.blockSize);

        out << "Block RBN: " << rbn << "\n";
        out << "Raw bytes: ";
        for (int i = 0; i < 16 && i < buffer.size(); ++i) {
            out << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
        }
        out << "\n\n";
    }

    out << "---- End of Dump ----\n";
    out.close();
    return true;
}

