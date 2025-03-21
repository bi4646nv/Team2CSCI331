/**
 * @file HeaderBuffer.cpp
 * @brief Implementation of the HeaderBuffer class for managing file headers.
 *
 * This file implements functions to read and write file headers containing metadata
 * about the structure and properties of length-indicated zip code files.
 */

#include "HeaderBuffer.h"

void HeaderBuffer::writeHeader(const string& filename, const FileHeader& header) {
    ofstream file(filename, ios::binary | ios::app); // Append mode to keep existing data
    if (!file.is_open()) {
        cerr << "Error: Could not open file to write header: " << filename << endl;
        return;
    }

    file << header.fileType << "\n"
         << header.version << "\n"
         << header.headerSize << "\n"
         << header.sizeFormat << "\n"
         << header.indexFileName << "\n"
         << header.recordCount << "\n"
         << header.fieldCount << "\n";

    file.close();
}

HeaderBuffer::FileHeader HeaderBuffer::readHeader(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not open file to read header: " << filename << endl;
        return {};
    }

    FileHeader header;
    getline(file, header.fileType); // Read file type as string
    file >> header.version;
    file >> header.headerSize;
    file.ignore(); // Ignore newline character
    getline(file, header.sizeFormat);
    getline(file, header.indexFileName);
    file >> header.recordCount;
    file >> header.fieldCount;

    file.close(); // Close file when done
    return header;
}
