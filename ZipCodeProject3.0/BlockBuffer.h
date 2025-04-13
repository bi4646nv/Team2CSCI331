/**
 * @file BlockBuffer.h
 * @brief Definition of the BlockBuffer class for handling blocks of records
 */

#ifndef BLOCK_BUFFER_H
#define BLOCK_BUFFER_H

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ZipCodeRecord.h"
#include "RecordBuffer.h"

/**
 * @class BlockBuffer
 * @brief Class for reading and writing blocks in the blocked sequence set file
 */
class BlockBuffer {
private:
    std::string buffer;              ///< Internal buffer for the block
    int blockSize;                   ///< Size of a block in bytes
    int prevBlockRBN;                ///< RBN of the previous block
    int nextBlockRBN;                ///< RBN of the next block
    int recordCount;                 ///< Number of records in the block
    std::vector<ZipCodeRecord> records; ///< Records in the block
    int headerSize;                  ///< Size of the block header
    int recordSizeBytes;             ///< Number of bytes for record size
    bool isBinary;                   ///< Flag for binary or ASCII format

    /**
     * @brief Parse block header from buffer
     */
    void parseHeader() {
        if (buffer.size() < 12) {
            return;
        };

#endif // BLOCK_BUFFER_H
        
        // Extract record count and RBN links
        std::string countStr = buffer.substr(0, 4);
        std::string prevStr = buffer.substr(4, 4);
        std::string nextStr = buffer.substr(8, 4);
        
        try {
            recordCount = std::stoi(countStr);
            prevBlockRBN = std::stoi(prevStr);
            nextBlockRBN = std::stoi(nextStr);
        } catch (...) {
            recordCount = 0;
            prevBlockRBN = -1;
            nextBlockRBN = -1;
        }
    }
    
    /**
     * @brief Create block header for buffer
     */
    void createHeader() {
        // Create fixed-size strings for record count and RBN links
        std::string countStr = std::to_string(recordCount);
        countStr.insert(0, 4 - countStr.size(), '0');
        
        std::string prevStr = std::to_string(prevBlockRBN);
        prevStr.insert(0, 4 - prevStr.size(), '0');
        
        std::string nextStr = std::to_string(nextBlockRBN);
        nextStr.insert(0, 4 - nextStr.size(), '0');
        
        // Combine into header
        std::string header = countStr + prevStr + nextStr;
        
        // Update buffer with header
        if (buffer.size() >= header.size()) {
            buffer.replace(0, header.size(), header);
        } else {
            buffer = header + buffer.substr(header.size());
        }
    }

public:
    /**
     * @brief Constructor
     * @param block_size Size of a block in bytes
     * @param rec_size_bytes Number of bytes for record size
     * @param is_binary Flag for binary or ASCII format
     */
    BlockBuffer(int block_size = 512, int rec_size_bytes = 4, bool is_binary = false)
        : blockSize(block_size), prevBlockRBN(-1), nextBlockRBN(-1), recordCount(0),
          headerSize(12), recordSizeBytes(rec_size_bytes), isBinary(is_binary) {
        buffer.resize(blockSize, ' ');
        createHeader();
    }
    
    /**
     * @brief Read a block from the file
     * @param file Input file stream
     * @param rbn Relative Block Number
     * @param header_size Size of the file header
     * @return true if successful, false otherwise
     */
    bool read(std::ifstream& file, int rbn, int header_size) {
        if (!file.is_open() || rbn < 0) {
            return false;
        }
        
        // Calculate file position
        std::streampos pos = header_size + static_cast<std::streampos>(rbn) * blockSize;
        file.seekg(pos);
        
        // Read block into buffer
        buffer.resize(blockSize);
        file.read(&buffer[0], blockSize);
        
        if (!file) {
            return false;
        }
        
        // Parse the header and unpack records
        parseHeader();
        unpackRecords();
        
        return true;
    }
    
    /**
     * @brief Write a block to the file
     * @param file Output file stream
     * @param rbn Relative Block Number
     * @param header_size Size of the file header
     * @return true if successful, false otherwise
     */
    bool write(std::ofstream& file, int rbn, int header_size) {
        if (!file.is_open() || rbn < 0) {
            return false;
        }
        
        // Pack records into buffer
        packRecords();
        
        // Calculate file position
        std::streampos pos = header_size + static_cast<std::streampos>(rbn) * blockSize;
        file.seekp(pos);
        
        // Write buffer to file
        file.write(buffer.c_str(), blockSize);
        
        return file.good();
    }
    
    /**
     * @brief Pack records into the buffer
     */
    void packRecords() {
        // Update record count
        recordCount = records.size();
        
        // Create header
        createHeader();
        
        // Start with a fresh buffer (after header)
        buffer.resize(blockSize, ' ');
        buffer.replace(0, headerSize, buffer.substr(0, headerSize));
        
        // Position in buffer after header
        int pos = headerSize;
        
        // Pack each record
        for (const auto& record : records) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            recBuffer.pack(record);
            std::string recStr = recBuffer.getBuffer();
            
            // Check if record fits in the block
            if (pos + recStr.size() > blockSize) {
                break;
            }
            
            // Add record to buffer
            buffer.replace(pos, recStr.size(), recStr);
            pos += recStr.size();
        }
    }
    
    /**
     * @brief Unpack records from the buffer
     */
    void unpackRecords() {
        records.clear();
        
        if (recordCount <= 0) {
            return;
        }
        
        // Position in buffer after header
        int pos = headerSize;
        
        // Unpack each record
        for (int i = 0; i < recordCount; i++) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            
            // Extract record length
            int recLen = 0;
            if (isBinary) {
                for (int j = 0; j < recordSizeBytes; j++) {
                    recLen = (recLen << 8) | static_cast<unsigned char>(buffer[pos + j]);
                }
            } else {
                std::string sizeStr = buffer.substr(pos, recordSizeBytes);
                recLen = std::stoi(sizeStr);
            }
            
            // Extract record
            int totalLen = recordSizeBytes + recLen;
            if (pos + totalLen > blockSize) {
                break;
            }
            
            recBuffer.setBuffer(buffer.substr(pos, totalLen));
            records.push_back(recBuffer.unpack());
            
            pos += totalLen;
        }
    }
    
    /**
     * @brief Add a record to the block
     * @param record The record to add
     * @return true if successful, false if block is full
     */
    bool addRecord(const ZipCodeRecord& record) {
        // Create a temporary record buffer to check size
        RecordBuffer recBuffer(recordSizeBytes, isBinary);
        recBuffer.pack(record);
        int recSize = recBuffer.getLength();
        
        // Calculate current used space in the block
        int usedSpace = headerSize;
        for (const auto& rec : records) {
            RecordBuffer tempBuf(recordSizeBytes, isBinary);
            tempBuf.pack(rec);
            usedSpace += tempBuf.getLength();
        }
        
        // Check if there's enough space
        if (usedSpace + recSize > blockSize) {
            return false;
        }
        
        // Add the record and sort
        records.push_back(record);
        std::sort(records.begin(), records.end());
        
        return true;
    }
    
    /**
     * @brief Remove a record from the block
     * @param zipCode The Zip Code of the record to remove
     * @return true if record was found and removed, false otherwise
     */
    bool removeRecord(const std::string& zipCode) {
        for (auto it = records.begin(); it != records.end(); ++it) {
            if (it->getZipCode() == zipCode) {
                records.erase(it);
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Search for a record in the block
     * @param zipCode The Zip Code to search for
     * @param record Output parameter for the found record
     * @return true if record was found, false otherwise
     */
    bool findRecord(const std::string& zipCode, ZipCodeRecord& record) const {
        for (const auto& rec : records) {
            if (rec.getZipCode() == zipCode) {
                record = rec;
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Check if the block should contain a given Zip Code
     * @param zipCode The Zip Code to check
     * @return true if this block should contain the Zip Code, false otherwise
     */
    bool shouldContain(const std::string& zipCode) const {
        if (records.empty()) {
            return false;
        }
        
        // If this is the first block, it contains all keys up to the highest key
        if (prevBlockRBN == -1) {
            return zipCode <= records.back().getZipCode();
        }
        
        // If this is the last block, it contains all keys from the lowest key
        if (nextBlockRBN == -1) {
            return zipCode >= records.front().getZipCode();
        }
        
        // Otherwise, check range
        return zipCode >= records.front().getZipCode() && zipCode <= records.back().getZipCode();
    }
    
    /**
     * @brief Merge with another block
     * @param other The other block to merge with
     * @return true if successful, false if merged block would be too large
     */
    bool mergeWith(const BlockBuffer& other) {
        // Calculate total size after merge
        int totalSize = headerSize;
        for (const auto& rec : records) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            recBuffer.pack(rec);
            totalSize += recBuffer.getLength();
        }
        
        for (const auto& rec : other.getRecords()) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            recBuffer.pack(rec);
            totalSize += recBuffer.getLength();
        }
        
        // Check if merged block would be too large
        if (totalSize > blockSize) {
            return false;
        }
        
        // Add records from other block
        records.insert(records.end(), other.getRecords().begin(), other.getRecords().end());
        
        // Sort records
        std::sort(records.begin(), records.end());
        
        return true;
    }
    
    /**
     * @brief Split the block into two
     * @param newBlock Output parameter for the new block
     * @return true if successful, false otherwise
     */
    bool split(BlockBuffer& newBlock) {
        if (records.size() < 2) {
            return false;
        }
        
        // Find middle point
        int midpoint = records.size() / 2;
        
        // Move records to new block
        newBlock.records.assign(records.begin() + midpoint, records.end());
        records.erase(records.begin() + midpoint, records.end());
        
        // Set RBN links
        newBlock.setNextBlockRBN(getNextBlockRBN());
        newBlock.setPrevBlockRBN(-1);  // Will be set by caller
        setNextBlockRBN(-1);  // Will be set by caller
        
        return true;
    }
    
    /**
     * @brief Redistribute records with another block
     * @param other The other block to redistribute with
     * @return true if successful, false otherwise
     */
    bool redistributeWith(BlockBuffer& other) {
        // Combine all records
        std::vector<ZipCodeRecord> allRecords = records;
        allRecords.insert(allRecords.end(), other.records.begin(), other.records.end());
        
        // Sort records
        std::sort(allRecords.begin(), allRecords.end());
        
        // Clear both blocks
        records.clear();
        other.records.clear();
        
        // Find midpoint
        int midpoint = allRecords.size() / 2;
        
        // Distribute records
        records.assign(allRecords.begin(), allRecords.begin() + midpoint);
        other.records.assign(allRecords.begin() + midpoint, allRecords.end());
        
        return true;
    }
    
    /**
     * @brief Convert to an availability list block
     */
    void convertToAvailBlock() {
        records.clear();
        recordCount = 0;
        
        // Clear buffer except for header
        buffer.replace(headerSize, blockSize - headerSize, blockSize - headerSize, ' ');
        
        // Update header
        createHeader();
    }
    
    /**
     * @brief Check if this is an availability list block
     * @return true if this is an availability list block, false otherwise
     */
    bool isAvailBlock() const {
        return recordCount == 0;
    }
    
    /**
     * @brief Get the highest key in the block
     * @return The highest key (Zip Code) in the block, or empty string if block is empty
     */
    std::string getHighestKey() const {
        if (records.empty()) {
            return "";
        }
        return records.back().getZipCode();
    }
    
    /**
     * @brief Get the lowest key in the block
     * @return The lowest key (Zip Code) in the block, or empty string if block is empty
     */
    std::string getLowestKey() const {
        if (records.empty()) {
            return "";
        }
        return records.front().getZipCode();
    }
    
    // Getters and setters
    
    /**
     * @brief Get the RBN of the previous block
     * @return The RBN of the previous block
     */
    int getPrevBlockRBN() const { return prevBlockRBN; }
    
    /**
     * @brief Get the RBN of the next block
     * @return The RBN of the next block
     */
    int getNextBlockRBN() const { return nextBlockRBN; }
    
    /**
     * @brief Get the number of records in the block
     * @return The number of records
     */
    int getRecordCount() const { return recordCount; }
    
    /**
     * @brief Get the records in the block
     * @return The records
     */
    const std::vector<ZipCodeRecord>& getRecords() const { return records; }
    
    /**
     * @brief Set the RBN of the previous block
     * @param rbn The RBN of the previous block
     */
    void setPrevBlockRBN(int rbn) { prevBlockRBN = rbn; }
    
    /**
     * @brief Set the RBN of the next block
     * @param rbn The RBN of the next block
     */
    void setNextBlockRBN(int rbn) { nextBlockRBN = rbn; }
    
    /**
     * @brief Set the block size
     * @param size The block size
     */
    void setBlockSize(int size) { blockSize = size; }
    
    /**
     * @brief Get the available space in the block
     * @return The available space in bytes
     */
    int getAvailableSpace() const {
        int usedSpace = headerSize;
        for (const auto& rec : records) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            recBuffer.pack(rec);
            usedSpace += recBuffer.getLength();
        }
        return blockSize - usedSpace;
    }
    
    /**
     * @brief Get the usage percentage of the block
     * @return The usage percentage (0-100)
     */
    double getUsagePercentage() const {
        int usedSpace = headerSize;
        for (const auto& rec : records) {
            RecordBuffer recBuffer(recordSizeBytes, isBinary);
            recBuffer.pack(rec);
            usedSpace += recBuffer.getLength();
        }
        return 100.0 * usedSpace / blockSize;
    }
};