/**
 * @file HeaderRecordBuffer.h
 * @brief Definition of the HeaderRecordBuffer class for managing file metadata
 */

#ifndef HEADER_RECORD_BUFFER_H
#define HEADER_RECORD_BUFFER_H

#include <string>
#include <vector>
#include <fstream>

/**
 * @class HeaderRecordBuffer
 * @brief Class for reading and writing the blocked sequence set file header
 */
class HeaderRecordBuffer {
private:
    std::string fileStructureType;  ///< Type of file structure
    int version;                    ///< Version of the file structure
    int headerRecordSize;           ///< Size of the header record in bytes
    int recordSizeBytes;            ///< Number of bytes for record size
    std::string sizeFormatType;     ///< Format of size (ASCII or binary)
    int blockSize;                  ///< Size of blocks in bytes
    double minBlockCapacity;        ///< Minimum block capacity (default 50%)
    std::string indexFileName;      ///< Name of the index file
    std::string indexFileSchema;    ///< Schema of the index file
    int recordCount;                ///< Total number of records in the file
    int blockCount;                 ///< Total number of blocks in the file
    int fieldsPerRecord;            ///< Number of fields per record
    std::vector<std::string> fieldNames;    ///< Names of each field
    std::vector<std::string> fieldTypes;    ///< Types of each field
    int primaryKeyField;            ///< Index of the primary key field
    int availListHead;              ///< RBN of the first available block
    int activeListHead;             ///< RBN of the first active block
    bool staleFlag;                 ///< Flag indicating if header is stale

public:
    /**
     * @brief Default constructor with default values
     */
    HeaderRecordBuffer()
        : fileStructureType("blocked_sequence_set_comma_separated_length_indicated"),
          version(1),
          headerRecordSize(0),  // Will be calculated
          recordSizeBytes(4),   // Default: 4 bytes for record size
          sizeFormatType("ASCII"),
          blockSize(512),       // Default: 512 bytes per block
          minBlockCapacity(0.5), // Default: 50% minimum block capacity
          indexFileName(""),
          indexFileSchema(""),
          recordCount(0),
          blockCount(0),
          fieldsPerRecord(6),   // Default for Zip Code records
          primaryKeyField(0),   // Zip Code is the primary key (first field)
          availListHead(-1),    // -1 indicates empty avail list
          activeListHead(-1),   // -1 indicates empty active list
          staleFlag(false) {
        
        // Default field names and types for Zip Code records
        fieldNames = {"ZipCode", "City", "State", "County", "Latitude", "Longitude"};
        fieldTypes = {"string", "string", "string", "string", "double", "double"};
    }

    /**
     * @brief Write the header to a file
     * @param file Output file stream
     * @return true if write successful, false otherwise
     */
    bool write(std::ofstream& file) {
        if (!file.is_open()) return false;
        
        // Reset file position to beginning
        file.seekp(0);
        
        // Create header string
        std::string header = "FILE_STRUCTURE=" + fileStructureType + "\n" +
                            "VERSION=" + std::to_string(version) + "\n" +
                            "HEADER_SIZE=" + std::to_string(calculateHeaderSize()) + "\n" +
                            "RECORD_SIZE_BYTES=" + std::to_string(recordSizeBytes) + "\n" +
                            "SIZE_FORMAT=" + sizeFormatType + "\n" +
                            "BLOCK_SIZE=" + std::to_string(blockSize) + "\n" +
                            "MIN_BLOCK_CAPACITY=" + std::to_string(minBlockCapacity) + "\n" +
                            "INDEX_FILE=" + indexFileName + "\n" +
                            "INDEX_SCHEMA=" + indexFileSchema + "\n" +
                            "RECORD_COUNT=" + std::to_string(recordCount) + "\n" +
                            "BLOCK_COUNT=" + std::to_string(blockCount) + "\n" +
                            "FIELDS_PER_RECORD=" + std::to_string(fieldsPerRecord) + "\n";
        
        // Add field information
        for (int i = 0; i < fieldsPerRecord; i++) {
            header += "FIELD_" + std::to_string(i) + "_NAME=" + fieldNames[i] + "\n";
            header += "FIELD_" + std::to_string(i) + "_TYPE=" + fieldTypes[i] + "\n";
        }
        
        header += "PRIMARY_KEY_FIELD=" + std::to_string(primaryKeyField) + "\n";
        header += "AVAIL_LIST_HEAD=" + std::to_string(availListHead) + "\n";
        header += "ACTIVE_LIST_HEAD=" + std::to_string(activeListHead) + "\n";
        header += "STALE_FLAG=" + std::to_string(staleFlag) + "\n";
        
        // Add padding to reach header size
        int paddingSize = calculateHeaderSize() - header.size();
        if (paddingSize > 0) {
            header.append(paddingSize, ' ');
        }
        
        // Write header to file
        file.write(header.c_str(), header.size());
        
        return file.good();
    }

    /**
     * @brief Read the header from a file
     * @param file Input file stream
     * @return true if read successful, false otherwise
     */
    bool read(std::ifstream& file) {
        if (!file.is_open()) return false;
        
        // Reset file position to beginning
        file.seekg(0);
        
        // Read the first line to get header size
        std::string line;
        int estimatedHeaderSize = 1024;  // Initial guess
        char* buffer = new char[estimatedHeaderSize];
        file.read(buffer, estimatedHeaderSize);
        
        if (!file) {
            // If we couldn't read the estimated size, read what we could
            estimatedHeaderSize = file.gcount();
        }
        
        std::string headerStr(buffer, estimatedHeaderSize);
        delete[] buffer;
        
        // Parse the header
        size_t pos = 0;
        std::string token;
        std::string delimiter = "\n";
        
        while ((pos = headerStr.find(delimiter)) != std::string::npos) {
            token = headerStr.substr(0, pos);
            headerStr.erase(0, pos + delimiter.length());
            
            // Parse key-value pair
            size_t equalPos = token.find('=');
            if (equalPos != std::string::npos) {
                std::string key = token.substr(0, equalPos);
                std::string value = token.substr(equalPos + 1);
                
                if (key == "FILE_STRUCTURE") fileStructureType = value;
                else if (key == "VERSION") version = std::stoi(value);
                else if (key == "HEADER_SIZE") headerRecordSize = std::stoi(value);
                else if (key == "RECORD_SIZE_BYTES") recordSizeBytes = std::stoi(value);
                else if (key == "SIZE_FORMAT") sizeFormatType = value;
                else if (key == "BLOCK_SIZE") blockSize = std::stoi(value);
                else if (key == "MIN_BLOCK_CAPACITY") minBlockCapacity = std::stod(value);
                else if (key == "INDEX_FILE") indexFileName = value;
                else if (key == "INDEX_SCHEMA") indexFileSchema = value;
                else if (key == "RECORD_COUNT") recordCount = std::stoi(value);
                else if (key == "BLOCK_COUNT") blockCount = std::stoi(value);
                else if (key == "FIELDS_PER_RECORD") {
                    fieldsPerRecord = std::stoi(value);
                    fieldNames.resize(fieldsPerRecord);
                    fieldTypes.resize(fieldsPerRecord);
                }
                else if (key.find("FIELD_") == 0 && key.find("_NAME") != std::string::npos) {
                    int fieldIdx = std::stoi(key.substr(6, key.find("_NAME") - 6));
                    if (fieldIdx < fieldsPerRecord) fieldNames[fieldIdx] = value;
                }
                else if (key.find("FIELD_") == 0 && key.find("_TYPE") != std::string::npos) {
                    int fieldIdx = std::stoi(key.substr(6, key.find("_TYPE") - 6));
                    if (fieldIdx < fieldsPerRecord) fieldTypes[fieldIdx] = value;
                }
                else if (key == "PRIMARY_KEY_FIELD") primaryKeyField = std::stoi(value);
                else if (key == "AVAIL_LIST_HEAD") availListHead = std::stoi(value);
                else if (key == "ACTIVE_LIST_HEAD") activeListHead = std::stoi(value);
                else if (key == "STALE_FLAG") staleFlag = (std::stoi(value) != 0);
            }
        }
        
        // Move the file pointer to after the header
        file.seekg(headerRecordSize);
        
        return file.good();
    }

    /**
     * @brief Calculate the size of the header
     * @return The size of the header in bytes
     */
    int calculateHeaderSize() {
        if (headerRecordSize > 0) return headerRecordSize;
        
        // Calculate based on content
        int size = 0;
        size += ("FILE_STRUCTURE=" + fileStructureType + "\n").size();
        size += ("VERSION=" + std::to_string(version) + "\n").size();
        size += ("HEADER_SIZE=0000\n").size();  // Placeholder
        size += ("RECORD_SIZE_BYTES=" + std::to_string(recordSizeBytes) + "\n").size();
        size += ("SIZE_FORMAT=" + sizeFormatType + "\n").size();
        size += ("BLOCK_SIZE=" + std::to_string(blockSize) + "\n").size();
        size += ("MIN_BLOCK_CAPACITY=" + std::to_string(minBlockCapacity) + "\n").size();
        size += ("INDEX_FILE=" + indexFileName + "\n").size();
        size += ("INDEX_SCHEMA=" + indexFileSchema + "\n").size();
        size += ("RECORD_COUNT=" + std::to_string(recordCount) + "\n").size();
        size += ("BLOCK_COUNT=" + std::to_string(blockCount) + "\n").size();
        size += ("FIELDS_PER_RECORD=" + std::to_string(fieldsPerRecord) + "\n").size();
        
        for (int i = 0; i < fieldsPerRecord; i++) {
            size += ("FIELD_" + std::to_string(i) + "_NAME=" + fieldNames[i] + "\n").size();
            size += ("FIELD_" + std::to_string(i) + "_TYPE=" + fieldTypes[i] + "\n").size();
        }
        
        size += ("PRIMARY_KEY_FIELD=" + std::to_string(primaryKeyField) + "\n").size();
        size += ("AVAIL_LIST_HEAD=" + std::to_string(availListHead) + "\n").size();
        size += ("ACTIVE_LIST_HEAD=" + std::to_string(activeListHead) + "\n").size();
        size += ("STALE_FLAG=" + std::to_string(staleFlag) + "\n").size();
        
        // Add some padding for safety
        size += 100;
        
        // Round up to the nearest multiple of 512 bytes
        if (size % 512 != 0) {
            size = ((size / 512) + 1) * 512;
        }
        
        return size;
    }

    // Getters and setters
    
    /**
     * @brief Get the file structure type
     * @return The file structure type
     */
    std::string getFileStructureType() const { return fileStructureType; }
    
    /**
     * @brief Get the version
     * @return The version
     */
    int getVersion() const { return version; }
    
    /**
     * @brief Get the header record size
     * @return The header record size
     */
    int getHeaderRecordSize() const { return headerRecordSize; }
    
    /**
     * @brief Get the number of bytes for record size
     * @return The number of bytes for record size
     */
    int getRecordSizeBytes() const { return recordSizeBytes; }
    
    /**
     * @brief Get the size format type
     * @return The size format type
     */
    std::string getSizeFormatType() const { return sizeFormatType; }
    
    /**
     * @brief Get the block size
     * @return The block size
     */
    int getBlockSize() const { return blockSize; }
    
    /**
     * @brief Get the minimum block capacity
     * @return The minimum block capacity
     */
    double getMinBlockCapacity() const { return minBlockCapacity; }
    
    /**
     * @brief Get the index file name
     * @return The index file name
     */
    std::string getIndexFileName() const { return indexFileName; }
    
    /**
     * @brief Get the index file schema
     * @return The index file schema
     */
    std::string getIndexFileSchema() const { return indexFileSchema; }
    
    /**
     * @brief Get the record count
     * @return The record count
     */
    int getRecordCount() const { return recordCount; }
    
    /**
     * @brief Get the block count
     * @return The block count
     */
    int getBlockCount() const { return blockCount; }
    
    /**
     * @brief Get the number of fields per record
     * @return The number of fields per record
     */
    int getFieldsPerRecord() const { return fieldsPerRecord; }
    
    /**
     * @brief Get the field names
     * @return The field names
     */
    const std::vector<std::string>& getFieldNames() const { return fieldNames; }
    
    /**
     * @brief Get the field types
     * @return The field types
     */
    const std::vector<std::string>& getFieldTypes() const { return fieldTypes; }
    
    /**
     * @brief Get the primary key field index
     * @return The primary key field index
     */
    int getPrimaryKeyField() const { return primaryKeyField; }
    
    /**
     * @brief Get the avail list head
     * @return The avail list head
     */
    int getAvailListHead() const { return availListHead; }
    
    /**
     * @brief Get the active list head
     * @return The active list head
     */
    int getActiveListHead() const { return activeListHead; }
    
    /**
     * @brief Check if the header is stale
     * @return true if the header is stale, false otherwise
     */
    bool isStale() const { return staleFlag; }
    
    /**
     * @brief Set the file structure type
     * @param type The file structure type
     */
    void setFileStructureType(const std::string& type) { fileStructureType = type; }
    
    /**
     * @brief Set the version
     * @param ver The version
     */
    void setVersion(int ver) { version = ver; }
    
    /**
     * @brief Set the header record size
     * @param size The header record size
     */
    void setHeaderRecordSize(int size) { headerRecordSize = size; }
    
    /**
     * @brief Set the number of bytes for record size
     * @param bytes The number of bytes for record size
     */
    void setRecordSizeBytes(int bytes) { recordSizeBytes = bytes; }
    
    /**
     * @brief Set the size format type
     * @param format The size format type
     */
    void setSizeFormatType(const std::string& format) { sizeFormatType = format; }
    
    /**
     * @brief Set the block size
     * @param size The block size
     */
    void setBlockSize(int size) { blockSize = size; }
    
    /**
     * @brief Set the minimum block capacity
     * @param capacity The minimum block capacity
     */
    void setMinBlockCapacity(double capacity) { minBlockCapacity = capacity; }
    
    /**
     * @brief Set the index file name
     * @param name The index file name
     */
    void setIndexFileName(const std::string& name) { indexFileName = name; }
    
    /**
     * @brief Set the index file schema
     * @param schema The index file schema
     */
    void setIndexFileSchema(const std::string& schema) { indexFileSchema = schema; }
    
    /**
     * @brief Set the record count
     * @param count The record count
     */
    void setRecordCount(int count) { recordCount = count; }
    
    /**
     * @brief Set the block count
     * @param count The block count
     */
    void setBlockCount(int count) { blockCount = count; }
    
    /**
     * @brief Set the number of fields per record
     * @param count The number of fields per record
     */
    void setFieldsPerRecord(int count) { 
        fieldsPerRecord = count; 
        fieldNames.resize(count);
        fieldTypes.resize(count);
    }
    
    /**
     * @brief Set the field names
     * @param names The field names
     */
    void setFieldNames(const std::vector<std::string>& names) { fieldNames = names; }
    
    /**
     * @brief Set the field types
     * @param types The field types
     */
    void setFieldTypes(const std::vector<std::string>& types) { fieldTypes = types; }
    
    /**
     * @brief Set the primary key field index
     * @param index The primary key field index
     */
    void setPrimaryKeyField(int index) { primaryKeyField = index; }
    
    /**
     * @brief Set the avail list head
     * @param head The avail list head
     */
    void setAvailListHead(int head) { availListHead = head; }
    
    /**
     * @brief Set the active list head
     * @param head The active list head
     */
    void setActiveListHead(int head) { activeListHead = head; }
    
    /**
     * @brief Set the stale flag
     * @param stale The stale flag
     */
    void setStale(bool stale) { staleFlag = stale; }
};

#endif // HEADER_RECORD_BUFFER_H
