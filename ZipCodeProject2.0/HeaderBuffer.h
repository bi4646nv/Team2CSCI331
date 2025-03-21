/**
 * @file HeaderBuffer.h
 * @brief Header file for managing file headers in length-indicated files.
 *
 * Provides methods for reading, writing, and printing file strcuture metadata.
 */

#ifndef HEADERBUFFER_H
#define HEADERBUFFER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
 
using namespace std;
 
/**
 * @class HeaderBuffer
 * @brief Handles reading and writing file headers for length-indicated zip code files.
 */
class HeaderBuffer {
public:
    /**
     * @struct FileHeader
     * @brief Structure for storing metadata about the file format.
     */
    struct FileHeader {
        string fileType; ///< File structure type
        int version; ///< Version of the file structure format
        int headerSize; ///< Total size of the header in bytes
        int recordSizeBytes; ///< Number of bytes used to store record length
        string sizeFormat; ///< Encoding format of size field (e.g., "ASCII" or "Binary")
        string indexFileName; ///< Name of the primary key index file
        int recordCount; ///< Total number of records in the data file
        int fieldCount; ///< Number of fields per record
        vector<pair<string, string>> fieldSchemas; ///< Vector of (field name, type) pairs
        int primaryKeyField; ///< Ordinal position of primary key field
    };
 
    /**
     * @brief Writes a header record to the specified file.
     * @param filename The file to write the header to.
     * @param header The header structure containing file metadata.
     */
    static void writeHeader(const string& filename, const FileHeader& header);
 
    /**
     * @brief Reads a header record from the specified file.
     * @param filename The file to read the header from.
     * @return The FileHeader structure containing the read metadata.
     *
     * If the file cannot be opened, an empty FileHeader structure is returned.
     */
    static FileHeader readHeader(const string& filename);

    /**
     * @brief Prints a file header to standard output in a readable format.
     * @param header The header structure to print.
     */
    static void printHeader(const FileHeader& header);

};
 
#endif // HEADERBUFFER_H
 