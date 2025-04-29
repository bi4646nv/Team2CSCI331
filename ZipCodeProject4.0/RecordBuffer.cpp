#include "RecordBuffer.h"
#include <sstream>
#include <iomanip>

/**
 * @brief Constructor for RecordBuffer
 * @param recordSizeBytes Number of bytes used to represent the record size
 * @param isBinaryFormat True for binary size representation, false for ASCII
 */
RecordBuffer::RecordBuffer(int recordSizeBytes, bool isBinaryFormat)
    : sizeBytes(recordSizeBytes), isBinary(isBinaryFormat) {}

/**
 * @brief Pack a ZipCodeRecord into the internal buffer with size metadata
 * @param record The record to pack
 * @return True if packing was successful
 */
bool RecordBuffer::pack(const ZipCodeRecord& record) {
    // Convert the record into CSV string format
    std::string csvRecord = record.toCSV();
    
    std::string sizeStr;
    if (isBinary) {
        // Create binary representation of record size
        int size = csvRecord.size();
        char sizeArr[sizeBytes];
        for (int i = 0; i < sizeBytes; i++) {
            sizeArr[sizeBytes - 1 - i] = (size >> (i * 8)) & 0xFF; // Little-endian
        }
        sizeStr = std::string(sizeArr, sizeBytes);
    } else {
        // Create ASCII (text) representation of size, padded with zeros
        std::stringstream ss;
        ss << std::setw(sizeBytes) << std::setfill('0') << csvRecord.size();
        sizeStr = ss.str();
    }

    // Combine the size and CSV string into the buffer
    buffer = sizeStr + csvRecord;
    return true;
}

/**
 * @brief Unpack the internal buffer into a ZipCodeRecord
 * @return The extracted ZipCodeRecord object
 */
ZipCodeRecord RecordBuffer::unpack() const {
    int recordSize = 0;

    if (isBinary) {
        // Read binary-encoded record size
        for (int i = 0; i < sizeBytes; i++) {
            recordSize = (recordSize << 8) | static_cast<unsigned char>(buffer[i]);
        }
    } else {
        // Read ASCII-encoded record size
        std::string sizeStr = buffer.substr(0, sizeBytes);
        recordSize = std::stoi(sizeStr);
    }

    // Extract the CSV record string from the buffer
    std::string csvRecord = buffer.substr(sizeBytes, recordSize);

    // Convert CSV back to a ZipCodeRecord object
    return ZipCodeRecord::fromCSV(csvRecord);
}

/**
 * @brief Get the total byte length of the packed record (size metadata + data)
 * @return Total number of bytes in the buffer
 */
int RecordBuffer::getLength() const {
    if (buffer.empty()) {
        return 0;
    }

    int recordSize = 0;
    if (isBinary) {
        for (int i = 0; i < sizeBytes; i++) {
            recordSize = (recordSize << 8) | static_cast<unsigned char>(buffer[i]);
        }
    } else {
        std::string sizeStr = buffer.substr(0, sizeBytes);
        recordSize = std::stoi(sizeStr);
    }

    // Return total length: metadata size + record data size
    return sizeBytes + recordSize;
}

/**
 * @brief Set the internal buffer to a raw string (e.g., from file or stream)
 * @param data The raw string data to set
 */
void RecordBuffer::setBuffer(const std::string& data) {
    buffer = data;
}

/**
 * @brief Get the internal buffer contents
 * @return The buffer as a std::string
 */
std::string RecordBuffer::getBuffer() const {
    return buffer;
}

/**
 * @brief Clear the internal buffer contents
 */
void RecordBuffer::clear() {
    buffer.clear();
}
