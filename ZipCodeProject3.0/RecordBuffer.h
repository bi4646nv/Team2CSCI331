/**
 * @file RecordBuffer.h
 * @brief Definition of the RecordBuffer class for handling record data
 */

#ifndef RECORD_BUFFER_H
#define RECORD_BUFFER_H

#include <string>
#include <sstream>
#include <iomanip>
#include "ZipCodeRecord.h"

/**
 * @class RecordBuffer
 * @brief Class for reading and writing Zip Code records in the blocked sequence set
 */
class RecordBuffer {
private:
    std::string buffer;       ///< Internal buffer for the record
    int sizeBytes;            ///< Number of bytes used for record size
    bool isBinary;            ///< Flag for binary or ASCII format

public:
    /**
     * @brief Default constructor
     * @param recordSizeBytes Number of bytes used for record size
     * @param isBinaryFormat Flag for binary or ASCII format
     */
    RecordBuffer(int recordSizeBytes = 4, bool isBinaryFormat = false)
        : sizeBytes(recordSizeBytes), isBinary(isBinaryFormat) {}

    /**
     * @brief Pack a ZipCodeRecord into the buffer
     * @param record The ZipCodeRecord to pack
     * @return true if packing was successful, false otherwise
     */
    bool pack(const ZipCodeRecord& record) {
        // Convert record to CSV format
        std::string csvRecord = record.toCSV();
        
        // Calculate record size as string
        std::string sizeStr;
        if (isBinary) {
            // Binary size format
            int size = csvRecord.size();
            char sizeArr[sizeBytes];
            for (int i = 0; i < sizeBytes; i++) {
                sizeArr[sizeBytes - 1 - i] = (size >> (i * 8)) & 0xFF;
            }
            sizeStr = std::string(sizeArr, sizeBytes);
        } else {
            // ASCII size format (padded with zeros)
            std::stringstream ss;
            ss << std::setw(sizeBytes) << std::setfill('0') << csvRecord.size();
            sizeStr = ss.str();
        }
        
        // Pack the buffer with size and record
        buffer = sizeStr + csvRecord;
        
        return true;
    }

    /**
     * @brief Unpack the buffer into a ZipCodeRecord
     * @return A ZipCodeRecord object
     */
    ZipCodeRecord unpack() const {
        // Extract the record size
        int recordSize = 0;
        if (isBinary) {
            // Binary size format
            for (int i = 0; i < sizeBytes; i++) {
                recordSize = (recordSize << 8) | static_cast<unsigned char>(buffer[i]);
            }
        } else {
            // ASCII size format
            std::string sizeStr = buffer.substr(0, sizeBytes);
            recordSize = std::stoi(sizeStr);
        }
        
        // Extract the CSV record
        std::string csvRecord = buffer.substr(sizeBytes, recordSize);
        
        // Create and return the ZipCodeRecord
        return ZipCodeRecord::fromCSV(csvRecord);
    }

    /**
     * @brief Get the full length of the record (size bytes + record)
     * @return The total length in bytes
     */
    int getLength() const {
        if (buffer.empty()) {
            return 0;
        }
        
        // Calculate record size
        int recordSize = 0;
        if (isBinary) {
            // Binary size format
            for (int i = 0; i < sizeBytes; i++) {
                recordSize = (recordSize << 8) | static_cast<unsigned char>(buffer[i]);
            }
        } else {
            // ASCII size format
            std::string sizeStr = buffer.substr(0, sizeBytes);
            recordSize = std::stoi(sizeStr);
        }
        
        // Return total length (size bytes + record size)
        return sizeBytes + recordSize;
    }

    /**
     * @brief Set the buffer from raw data
     * @param data The raw data
     */
    void setBuffer(const std::string& data) {
        buffer = data;
    }

    /**
     * @brief Get the buffer
     * @return The buffer as a string
     */
    std::string getBuffer() const {
        return buffer;
    }

    /**
     * @brief Clear the buffer
     */
    void clear() {
        buffer.clear();
    }
};

#endif // RECORD_BUFFER_H
