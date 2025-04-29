#ifndef BPLUSTREE_H
#define BPLUSTREE_H

/**
 * @file BPlusTree.h
 * @brief Definition of the BPlusTree class for managing a B+ Tree index
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "IndexBlockBuffer.h"
#include "BlockBuffer.h"
#include "ZipCodeRecord.h"
/**
 * @class BPlusTree
 * @brief A file-based B+ Tree implementation
 * This class implements a B+ Tree that is stored on disk as a file
 * of linked, fixed-size blocks. It follows the Folk Section 9.10 specifications.
 */
class BPlusTree {
private:
    std::string filename;        ///< Name of the file that would store the B+ Tree
    std::fstream file;           ///< File stream for operations
    int blockSize;               ///< Size of each block in bytes
    int order;                  

 /**
     * @brief Header record structure
     */
    struct HeaderRecord {
        int rootRBN;             
        int height;              
        int totalBlocks;         ///< Total number of blocks in the file
        int firstLeafRBN;        ///< RBN of the first leaf block (for sequence set)
        int lastLeafRBN;         ///< RBN of the last leaf block (for sequence set)
        int blockSize;           ///< Size of each block in bytes
        int order;               ///< Order of the B+ tree
        int headerSize;          ///< Size of the file header in bytes
    };
    
    HeaderRecord header;         ///< Header record (kept in RAM)
    
    /**
     * @brief Writes the header record to the file
     * @return Will return true if successful, if not false
     */
    bool writeHeader();

    
    /**
     * @brief Reads the header record from the file
     * @return Will return true if successful, if not false
     */
    bool readHeader();

    
    /**
     * @brief Find the leaf block that should contain the key
     * @param key The key to search for
     * @return RBN of the leaf block
     */
    int findLeafBlock(const std::string& key);
    
    /**
     * @brief Gets the next available block in the file
     * @return RBN of the next available block
     */
    int getNextAvailableRBN();
    
public:
    /**
     * @brief Constructor
     */
    BPlusTree();
    
    /**
     * @brief Destructor
     */
    ~BPlusTree();
    
    /**
     * @brief Create a new B+ tree file
     * @param filename Name of the file to create
     * @param blockSize Size of each block in bytes
     * @param order Order of the B+ tree
     * @return will return true if successful, if not false
     */
    bool create(const std::string& filename, int blockSize, int order);
    
    /**
     * @brief Open an existing B+ tree file
     * @param filename Name of the file to open
     * @return will return true if successful, if not false
     */
    bool open(const std::string& filename);
    void close();
    
    /**
     * @brief Bulk load data into the B+ tree
     * @param dataFile Name of the data file to load
     * @return true if successful, false otherwise
     */
    bool bulkLoad(const std::string& dataFile);
    
    /**
     * @brief Search for a record by key
     * @param key Key to search for
     * @param record Output parameter for the found record
     * @return true if record found, false otherwise
     */
    bool search(const std::string& key, ZipCodeRecord& record);
    
    /**
     * @brief Search for records in a range of keys
     * @param startKey Start key of the range
     * @param endKey End key of the range
     * @param records Vector to store found records
     * @return true if at least one record found, false otherwise
     */
    bool rangeSearch(const std::string& startKey, const std::string& endKey, 
                    std::vector<ZipCodeRecord>& records);
    
    /**
     * @brief Insert a record into the B+ tree 
     * @param record to insert
     * @return will return true if successful, if not false
     */
    bool insert(const ZipCodeRecord& record);
    
    /**
     * @brief Deletes a record from the B+ tree
     * @param key of the record to delete
     * @return will return true if successful, if not false
     */
    bool remove(const std::string& key);
    
    /**
     * @brief Print the B+ tree structure
     */
    void print() const;
    
    /**
     * @brief Dump the B+ tree structure to a file
     * @param outputFile Name of the output file
     * @return true if successful, false otherwise
     */
    bool dumpTree(const std::string& outputFile) const;
    
    /**
     * @brief Gets the height of the tree
     * @return Returns the height of the tree
     */
    int getHeight() const { return header.height; }
    
    /**
     * @brief Gets the total number of blocks in the file
     * @return Total number of blocks
     */
    int getTotalBlocks() const { return header.totalBlocks; }
    
    /**
     * @brief Finds records by state code
     * @param Codes to search for
     * @param records Vector to store found records
     * @return true if at least one record found, false otherwise
     */
    bool findByState(const std::string& stateCode, std::vector<ZipCodeRecord>& records);
};

#endif // BPLUSTREE_H
