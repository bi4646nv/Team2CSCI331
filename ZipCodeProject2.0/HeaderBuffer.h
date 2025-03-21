/**
 * @file HeaderBuffer.h
 * @brief Header file for managing file headers in length-indicated files.
 *
 * This class provides methods to write and read file headers, which include metadata
 * such as file format type, version, index filename, record count, and field count.
 */

#ifndef HEADERBUFFER_H
#define HEADERBUFFER_H
 
#include <iostream>
#include <fstream>
#include <string>
 
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
        string fileType;      ///< File structure type
        int version;          ///< Version of the file format
        int headerSize;       ///< Size of the header record in bytes
        string sizeFormat;    ///< Size format type (ASCII/Binary)
        string indexFileName; ///< Primary key index file name
        int recordCount;      ///< Number of records in the file
        int fieldCount;       ///< Number of fields per record
    };
 
    /**
     * @brief Writes a header record to the specified file.
     * @param filename The file to write the header to.
     * @param header The header structure containing file metadata.
     * 
     * The header includes metadata such as file format type, version, record count, and index file name.
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
};
 
#endif // HEADERBUFFER_H
 