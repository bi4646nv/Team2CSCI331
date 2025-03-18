/**
 * @file HeaderBuffer.cpp
 * @brief Implementation of the HeaderBuffer class for managing file headers.
 */

#include "HeaderBuffer.h"

void HeaderBuffer::writeHeader(const string& filename, const FileHeader& header) {
    ofstream file(filename, ios::binary | ios::app);
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
    getline(file, header.fileType);
    file >> header.version;
    file >> header.headerSize;
    file.ignore(); // Ignore newline
    getline(file, header.sizeFormat);
    getline(file, header.indexFileName);
    file >> header.recordCount;
    file >> header.fieldCount;

    file.close();
    return header;
}

