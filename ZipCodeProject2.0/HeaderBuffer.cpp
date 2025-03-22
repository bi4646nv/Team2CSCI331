/**
 * @file HeaderBuffer.cpp
 * @brief Implementation of the HeaderBuffer class for managing file headers.
 */

#include "HeaderBuffer.h"

/**
 * @brief Writes a structured header to the top of the file.
 * Replaces the old header while preserving existing records.
 */
void HeaderBuffer::writeHeader(const string& filename, const FileHeader& header) {
    string tempFile = filename + ".tmp";

    ifstream original(filename);
    ofstream temp(tempFile);

    if (!temp.is_open()) {
        cerr << "Error: Could not create temp file: " << tempFile << endl;
        return;
    }

    // Write fixed header fields
    temp << header.fileType << "\n"
         << header.version << "\n"
         << header.headerSize << "\n"
         << header.recordSizeBytes << "\n"
         << header.sizeFormat << "\n"
         << header.indexFileName << "\n"
         << header.recordCount << "\n"
         << header.fieldCount << "\n";

    // Write field schemas (name,type)
    for (const auto& field : header.fieldSchemas) {
        temp << field.first << "," << field.second << "\n";
    }

    // Write primary key index
    temp << header.primaryKeyField << "\n";

    // Safely skip header lines (only if they exist)
    int linesToSkip = 8 + header.fieldSchemas.size() + 1;
    string dummy;
    int skipped = 0;
    while (skipped < linesToSkip && getline(original, dummy)) {
        skipped++;
    }

    // Copy remaining data records
    string line;
    while (getline(original, line)) {
        temp << line << '\n';
    }

    // Finalize file replacement
    original.close();
    temp.close();
    remove(filename.c_str());
    rename(tempFile.c_str(), filename.c_str());
}

HeaderBuffer::FileHeader HeaderBuffer::readHeader(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file to read header: " << filename << endl;
        return {};
    }

    FileHeader header;
    getline(file, header.fileType);
    file >> header.version;
    file >> header.headerSize;
    file >> header.recordSizeBytes;
    file.ignore();
    getline(file, header.sizeFormat);
    getline(file, header.indexFileName);
    file >> header.recordCount;
    file >> header.fieldCount;
    file.ignore();

    // Read field schemas
    for (int i = 0; i < header.fieldCount; ++i) {
        string line;
        getline(file, line);
        size_t commaPos = line.find(',');
        if (commaPos != string::npos) {
            string name = line.substr(0, commaPos);
            string type = line.substr(commaPos + 1);
            header.fieldSchemas.push_back({name, type});
        }
    }

    file >> header.primaryKeyField; // Read primary key field position

    file.close(); // Close file
    return header;
}


void HeaderBuffer::printHeader(const FileHeader& header) {
    // Print header information to the terminal
    cout << "=== File Header ===" << endl;
    cout << "File Type: "    << header.fileType    << endl;
    cout << "Version: "      << header.version     << endl;
    cout << "Header Size: "  << header.headerSize  << endl;
    cout << "Record Size Bytes: " << header.recordSizeBytes << endl;
    cout << "Size Format: "  << header.sizeFormat  << endl;
    cout << "Index File: "   << header.indexFileName << endl;
    cout << "Record Count: " << header.recordCount << endl;
    cout << "Field Count: "  << header.fieldCount  << endl;
    cout << "Fields:" << endl;
    for (size_t i = 0; i < header.fieldSchemas.size(); ++i) {
        cout << "  " << i + 1 << ". " 
             << header.fieldSchemas[i].first 
             << " (" << header.fieldSchemas[i].second << ")\n";
    }
    
    cout << "Primary Key Field: " << header.primaryKeyField << endl;
    cout << "===================" << endl;
}
