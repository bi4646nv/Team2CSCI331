/**
 * @file BSSManager.h
 * @brief Definition of the BSSManager class for managing blocked sequence set files
 */

#ifndef BSS_MANAGER_H
#define BSS_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include "HeaderRecordBuffer.h"
#include "BlockBuffer.h"
#include "RecordBuffer.h"
#include "ZipCodeRecord.h"

/**
 * @class BSSManager
 * @brief Class for managing blocked sequence set files
 */
class BSSManager {
private:
    std::string dataFileName;        ///< Name of the data file
    std::string indexFileName;       ///< Name of the index file
    HeaderRecordBuffer header;       ///< Header record buffer
    std::map<std::string, int> index; ///< Index mapping highest keys to RBNs

    /**
     * @brief Read the index from file
     * @return true if successful, false otherwise
     */
    bool readIndex() {
        index.clear();
        
        std::ifstream file(indexFileName);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t commaPos = line.find(',');
            if (commaPos != std::string::npos) {
                std::string key = line.substr(0, commaPos);
                int rbn = std::stoi(line.substr(commaPos + 1));
                index[key] = rbn;
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * @brief Write the index to file
     * @return true if successful, false otherwise
     */
    bool writeIndex() {
        std::ofstream file(indexFileName);
        if (!file.is_open()) return false;
    
        std::map<std::string, int> sorted(index.begin(), index.end());
        for (const auto& pair : sorted) {
            file << pair.first << "," << pair.second << "\n";
        }
        return true;
    }
    
    /**
     * @brief Find a block by key using the index
     * @param key The key to search for
     * @return The RBN of the block that should contain the key
     */
    int findBlockByKey(const std::string& key) {
        if (index.empty()) readIndex();
    
        // Linear scan: find first block whose highest key is >= the search key
        for (const auto& pair : index) {
            if (key <= pair.first) {
                return pair.second;
            }
        }
    
        // If not found, return last block
        return std::prev(index.end())->second;
    }
    
    
    /**
     * @brief Update the index with a new highest key for a block
     * @param oldKey The old highest key
     * @param newKey The new highest key
     * @param rbn The RBN of the block
     */
    void updateIndex(const std::string& oldKey, const std::string& newKey, int rbn) {
        if (!oldKey.empty()) {
            index.erase(oldKey);
        }
        if (!newKey.empty()) {
            index[newKey] = rbn;
        }
        writeIndex();
    }
    
    /**
     * @brief Get a new block RBN, either from the avail list or by appending
     * @return The new RBN
     */
    int getNewBlockRBN() {
        int availHead = header.getAvailListHead();
        
        if (availHead >= 0) {
            // Use a block from the avail list
            BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
            std::ifstream file(dataFileName, std::ios::binary);
            block.read(file, availHead, header.getHeaderRecordSize());
            file.close();
            
            // Update avail list head
            header.setAvailListHead(block.getNextBlockRBN());
            
            return availHead;
        } else {
            // Append a new block
            return header.getBlockCount();
        }
    }
    
    /**
     * @brief Add a block to the avail list
     * @param rbn The RBN of the block to add
     */
    void addToAvailList(int rbn) {
        BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
        
        // Read the block
        std::ifstream readFile(dataFileName, std::ios::binary);
        block.read(readFile, rbn, header.getHeaderRecordSize());
        readFile.close();
        
        // Convert to avail block
        block.convertToAvailBlock();
        block.setNextBlockRBN(header.getAvailListHead());
        
        // Update avail list head
        header.setAvailListHead(rbn);
        
        // Write the block back
        std::ofstream writeFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
        block.write(writeFile, rbn, header.getHeaderRecordSize());
        writeFile.close();
        
        // Update header
        std::ofstream headerFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
        header.write(headerFile);
        headerFile.close();
    }

public:
    /**
     * @brief Constructor
     * @param dataFile Name of the data file
     * @param indexFile Name of the index file
     */
    BSSManager(const std::string& dataFile, const std::string& indexFile)
        : dataFileName(dataFile), indexFileName(indexFile) {
    }
    
    /**
     * @brief Initialize a new blocked sequence set file
     * @param blockSize Size of blocks in bytes
     * @return true if successful, false otherwise
     */
    bool initialize(int blockSize = 512) {
        // Set up header
        header.setBlockSize(blockSize);
        header.setIndexFileName(indexFileName);
        header.setRecordCount(0);
        header.setBlockCount(0);
        header.setAvailListHead(-1);
        header.setActiveListHead(-1);
        header.setHeaderRecordSize(header.calculateHeaderSize());
        
        // Create and write header to file
        std::ofstream file(dataFileName, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        
        bool success = header.write(file);
        file.close();
        
        // Create empty index file
        std::ofstream indexFile(indexFileName, std::ios::trunc);
        indexFile.close();
        
        return success;
    }
    
    /**
     * @brief Create a blocked sequence set file from a CSV file
     * @param csvFileName Name of the CSV file
     * @return true if successful, false otherwise
     */
    bool createFromCSV(const std::string& csvFileName) {
        // Open CSV file
        std::ifstream csvFile(csvFileName);
        if (!csvFile.is_open()) {
            std::cerr << "Error: Could not open CSV file " << csvFileName << std::endl;
            return false;
        }
        
        // Read CSV file
        std::vector<ZipCodeRecord> records;
        std::string line;
        
        // Skip header line
        std::getline(csvFile, line);
        
        // Read data lines
        while (std::getline(csvFile, line)) {
            ZipCodeRecord record = ZipCodeRecord::fromCSV(line);
            records.push_back(record);
        }
        
        csvFile.close();
        
        // Sort records by Zip Code
        std::sort(records.begin(), records.end());
        
        // Create blocked sequence set file
        std::ofstream dataFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!dataFile.is_open()) {
            std::cerr << "Error: Could not open data file " << dataFileName << std::endl;
            return false;
        }
        
        // Read header
        std::ifstream headerFile(dataFileName, std::ios::binary);
        header.read(headerFile);
        headerFile.close();
        
        // Process records
        int blockSize = header.getBlockSize();
        int recordSizeBytes = header.getRecordSizeBytes();
        bool isBinary = (header.getSizeFormatType() == "binary");
        
        BlockBuffer currentBlock(blockSize, recordSizeBytes, isBinary);
        int currentRBN = 0;
        int prevRBN = -1;
        
        for (const auto& record : records) {
            // If block is full, write it and create a new one
            if (!currentBlock.addRecord(record)) {
                // Update RBN links
                currentBlock.setPrevBlockRBN(prevRBN);
                currentBlock.setNextBlockRBN(currentRBN + 1);
                
                // Write block
                currentBlock.write(dataFile, currentRBN, header.getHeaderRecordSize());
                
                // Add to index
                index[currentBlock.getHighestKey()] = currentRBN;
                
                // Move to next block
                prevRBN = currentRBN;
                currentRBN++;
                
                // Create new block
                currentBlock = BlockBuffer(blockSize, recordSizeBytes, isBinary);
                currentBlock.addRecord(record);
            }
        }
        
        // Write the last block
        currentBlock.setPrevBlockRBN(prevRBN);
        currentBlock.setNextBlockRBN(-1);
        currentBlock.write(dataFile, currentRBN, header.getHeaderRecordSize());
        
        // Add to index
        index[currentBlock.getHighestKey()] = currentRBN;
        
        // Update header
        header.setRecordCount(records.size());
        header.setBlockCount(currentRBN + 1);
        header.setActiveListHead(0);
        header.write(dataFile);
        
        dataFile.close();
        
        // Write index
        return writeIndex();
    }
    
    /**
     * @brief Search for a record by Zip Code
     * @param zipCode The Zip Code to search for
     * @param result Output parameter for the found record
     * @return true if record was found, false otherwise
     */
    bool search(const std::string& zipCode, ZipCodeRecord& result) {
        // Find block using index
        int rbn = findBlockByKey(zipCode);
        
        // Read block
        BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
        std::ifstream file(dataFileName, std::ios::binary);
        block.read(file, rbn + 2, header.getHeaderRecordSize());
        file.close();
        
        std::cout << "Block RBN being searched: " << rbn << std::endl;
        for (const auto& r : block.getRecords()) {
            std::cout << "   contains zip: [" << r.getZipCode() << "]" << std::endl;
        }


        // Search for record in block
        return block.findRecord(zipCode, result);
    }
    
    /**
     * @brief Insert a record
     * @param record The record to insert
     * @return true if successful, false otherwise
     */
    bool insert(const ZipCodeRecord& record) {
        std::string zipCode = record.getZipCode();
        
        // Check if record already exists
        ZipCodeRecord existingRecord;
        if (search(zipCode, existingRecord)) {
            std::cerr << "Error: Record with Zip Code " << zipCode << " already exists" << std::endl;
            return false;
        }
        
        // Find block using index
        int rbn = findBlockByKey(zipCode);
        
        // Read block
        BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
        std::ifstream readFile(dataFileName, std::ios::binary);
        block.read(readFile, rbn, header.getHeaderRecordSize());
        readFile.close();
        
        // Try to add record to block
        if (block.addRecord(record)) {
            // Record fits in the block, just update the block
            std::ofstream writeFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
            block.write(writeFile, rbn, header.getHeaderRecordSize());
            writeFile.close();
            
            // Update index if highest key changed
            std::string oldHighest = block.getHighestKey();
            if (zipCode > oldHighest) {
                updateIndex(oldHighest, zipCode, rbn);
            }
            
            // Update record count
            header.setRecordCount(header.getRecordCount() + 1);
            std::ofstream headerFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
            header.write(headerFile);
            headerFile.close();
            
            return true;
        } else {
            // Record doesn't fit, need to split the block
            BlockBuffer newBlock(header.getBlockSize(), header.getRecordSizeBytes());
            if (!block.split(newBlock)) {
                std::cerr << "Error: Could not split block" << std::endl;
                return false;
            }
            
            // Get new RBN for the new block
            int newRBN = getNewBlockRBN();
            std::cout << "Block split: Block " << rbn << " split into blocks " << rbn << " and " << newRBN << std::endl;
            
            // Update RBN links
            int nextRBN = block.getNextBlockRBN();
            block.setNextBlockRBN(newRBN);
            newBlock.setPrevBlockRBN(rbn);
            newBlock.setNextBlockRBN(nextRBN);
            
            // If there was a next block, update its prev link
            if (nextRBN >= 0) {
                BlockBuffer nextBlock(header.getBlockSize(), header.getRecordSizeBytes());
                std::ifstream nextReadFile(dataFileName, std::ios::binary);
                nextBlock.read(nextReadFile, nextRBN, header.getHeaderRecordSize());
                nextReadFile.close();
                
                nextBlock.setPrevBlockRBN(newRBN);
                
                std::ofstream nextWriteFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
                nextBlock.write(nextWriteFile, nextRBN, header.getHeaderRecordSize());
                nextWriteFile.close();
            }
            
            // Try to add the record to the appropriate block
            bool added = false;
            if (zipCode <= block.getHighestKey()) {
                added = block.addRecord(record);
            } else {
                added = newBlock.addRecord(record);
            }
            
            if (!added) {
                std::cerr << "Error: Could not add record after split" << std::endl;
                return false;
            }
            
            // Write blocks
            std::ofstream writeFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
            block.write(writeFile, rbn, header.getHeaderRecordSize());
            newBlock.write(writeFile, newRBN, header.getHeaderRecordSize());
            writeFile.close();
            
            // Update index
            std::string oldHighest = block.getHighestKey();
            if (oldHighest != block.getHighestKey()) {
                index.erase(oldHighest);
                index[block.getHighestKey()] = rbn;
            }
            
            index[newBlock.getHighestKey()] = newRBN;
            
            
            // Update header
            header.setRecordCount(header.getRecordCount() + 1);
            header.setBlockCount(std::max(header.getBlockCount(), newRBN + 1));
            std::ofstream headerFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
            header.write(headerFile);
            headerFile.close();
            
            return true;
        }
    }
    
    /**
     * @brief Delete a record by Zip Code
     * @param zipCode The Zip Code of the record to delete
     * @return true if successful, false otherwise
     */
    bool remove(const std::string& zipCode) {
        // Find block using index
        int rbn = findBlockByKey(zipCode);
        
        // Read block
        BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
        std::ifstream readFile(dataFileName, std::ios::binary);
        block.read(readFile, rbn + 2, header.getHeaderRecordSize());
        readFile.close();
        
        // Try to remove the record
        if (!block.removeRecord(zipCode)) {
            std::cerr << "Error: Record with Zip Code " << zipCode << " not found" << std::endl;
            return false;
        }
        
        // Update record count
        header.setRecordCount(header.getRecordCount() - 1);
        
        // Check if block is now empty
        if (block.getRecordCount() == 0) {
            // Block is empty, should be merged or redistributed
            int prevRBN = block.getPrevBlockRBN();
            int nextRBN = block.getNextBlockRBN();
            
            // Update RBN links
            if (prevRBN >= 0) {
                BlockBuffer prevBlock(header.getBlockSize(), header.getRecordSizeBytes());
                std::ifstream prevReadFile(dataFileName, std::ios::binary);
                prevBlock.read(prevReadFile, prevRBN, header.getHeaderRecordSize());
                prevReadFile.close();
                
                prevBlock.setNextBlockRBN(nextRBN);
                
                std::ofstream prevWriteFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
                prevBlock.write(prevWriteFile, prevRBN, header.getHeaderRecordSize());
                prevWriteFile.close();
            } else {
                // This was the first block
                header.setActiveListHead(nextRBN);
            }
            
            if (nextRBN >= 0) {
                BlockBuffer nextBlock(header.getBlockSize(), header.getRecordSizeBytes());
                std::ifstream nextReadFile(dataFileName, std::ios::binary);
                nextBlock.read(nextReadFile, nextRBN, header.getHeaderRecordSize());
                nextReadFile.close();
                
                nextBlock.setPrevBlockRBN(prevRBN);
                
                std::ofstream nextWriteFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
                nextBlock.write(nextWriteFile, nextRBN, header.getHeaderRecordSize());
                nextWriteFile.close();
            }
            
            // Add block to avail list
            addToAvailList(rbn);
            
            // Update index
            updateIndex(block.getHighestKey(), "", -1);
            
        } else {
            // Block still has records, just update it
            std::string oldHighest = block.getHighestKey();
            
            // Write block
            std::ofstream writeFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
            block.write(writeFile, rbn + 2, header.getHeaderRecordSize());
            writeFile.close();
            
            // Update index if highest key changed
            std::string newHighest = block.getHighestKey();
            if (oldHighest != newHighest) {
                updateIndex(oldHighest, newHighest, rbn);
            }
        }
        
        // Update header
        std::ofstream headerFile(dataFileName, std::ios::binary | std::ios::in | std::ios::out);
        header.write(headerFile);
        headerFile.close();
        
        return true;
    }
    
    /**
     * @brief Helper function for logging to both out file and terminal
     */
    void logToBoth(std::ostream& fileStream, const std::string& message) {
        std::cout << message << std::endl;
        fileStream << message << std::endl;
    }
    
    /**
     * @brief Dump the physical structure of the file
     */
    void dumpPhysical(const std::string& outputFile = "dump_physical.txt") {
        std::ofstream out(outputFile);
        if (!out.is_open()) {
            std::cerr << "Failed to open " << outputFile << " for writing." << std::endl;
            return;
        }
    
        std::ifstream file(dataFileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open data file: " << dataFileName << std::endl;
            out << "Could not open data file: " << dataFileName << std::endl;
            return;
        }
    
        header.read(file);
    
        auto logToBoth = [&](const std::string& message) {
            std::cout << message << std::endl;
            out << message << std::endl;
        };
    
        logToBoth("List Head: " + std::to_string(header.getActiveListHead()));
        logToBoth("Avail Head: " + std::to_string(header.getAvailListHead()));
    
        for (int rbn = 0; rbn < header.getBlockCount(); rbn++) {
            BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
            block.read(file, rbn, header.getHeaderRecordSize());
    
            std::ostringstream line;
            line << "RBN " << std::setw(3) << rbn << "  ";
    
            if (block.isAvailBlock()) {
                line << "*available*     -> " << block.getNextBlockRBN();
            } else {
                for (const auto& record : block.getRecords()) {
                    line << record.getZipCode() << " ";
                }
                line << "-> " << block.getNextBlockRBN();  // Link to next
            }
    
            logToBoth(line.str());
        }
    
        out.close();
        file.close();
    }
    
    
    /**
     * @brief Dump the logical structure of the file
     */
    void dumpLogical(const std::string& outputFile = "dump_logical.txt") {
        std::ofstream out(outputFile);
        if (!out.is_open()) {
            std::cerr << "Failed to open " << outputFile << " for writing." << std::endl;
            return;
        }
    
        std::ifstream file(dataFileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open data file: " << dataFileName << std::endl;
            out << "Could not open data file: " << dataFileName << std::endl;
            return;
        }
    
        header.read(file);
    
        auto logToBoth = [&](const std::string& message) {
            std::cout << message << std::endl;
            out << message << std::endl;
        };
    
        logToBoth("List Head: " + std::to_string(header.getActiveListHead()));
        logToBoth("Avail Head: " + std::to_string(header.getAvailListHead()));
    
        std::set<int> visited; // Prevent infinite loops
    
        // Dump active list
        int rbn = header.getActiveListHead();
        while (rbn >= 0 && !visited.count(rbn)) {
            visited.insert(rbn);
    
            BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
            block.read(file, rbn, header.getHeaderRecordSize());
    
            std::ostringstream line;
            line << "RBN " << std::setw(3) << rbn << "  ";
            for (const auto& record : block.getRecords()) {
                line << record.getZipCode() << " ";
            }
            line << "-> " << block.getNextBlockRBN();
            logToBoth(line.str());
    
            rbn = block.getNextBlockRBN();
        }
    
        // Dump avail list
        rbn = header.getAvailListHead();
        while (rbn >= 0 && !visited.count(rbn)) {
            visited.insert(rbn);
    
            BlockBuffer block(header.getBlockSize(), header.getRecordSizeBytes());
            block.read(file, rbn, header.getHeaderRecordSize());
    
            std::ostringstream line;
            line << "RBN " << std::setw(3) << rbn << "  *available*     -> " << block.getNextBlockRBN();
            logToBoth(line.str());
    
            rbn = block.getNextBlockRBN();
        }
    
        file.close();
        out.close();
    }    
    
    /**
     * @brief Dump the index
     */
    void dumpIndex() {
        if (index.empty()) {
            readIndex();
        }
        
        std::cout << "Index: " << std::endl;
        for (const auto& pair : index) {
            std::cout << pair.first << " -> " << pair.second << std::endl;
        }
    }
};

#endif // BSS_MANAGER_H