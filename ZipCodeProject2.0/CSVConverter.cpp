/**
 * @file CSVConverter.cpp
 * @brief Implementation of CSVConverter for converting CSV to length-indicated format.
 */

#include "CSVConverter.h"
#include <iomanip>

void CSVConverter::convertToLengthIndicated(const string& csvFilename, const string& outputFilename) {
    ifstream inputFile(csvFilename);

    string modifiedOutputFilename = outputFilename;
    if (modifiedOutputFilename.find(".txt") != string::npos) {
        modifiedOutputFilename.replace(modifiedOutputFilename.find(".txt"), 4, ".csv");
    }

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input file: " << csvFilename << endl;
        return;
    }

    ofstream outputFile(modifiedOutputFilename, ios::trunc); // Create a blank file to prepare writing
    outputFile.close(); // File is cleared and ready

    // Define the file header metadata
    HeaderBuffer::FileHeader header = {
        "ZipCodeData",     // fileType
        1,                 // version
        128,               // headerSize (arbitrary)
        2,                 // recordSizeBytes (2-digit ASCII length prefix)
        "ASCII",           // sizeFormat
        "zip_index.txt",   // indexFileName
        0,                 // recordCount (placeholder, will update later)
        6,                 // fieldCount
        {
            {"zip_code", "int"},
            {"place_name", "string"},
            {"state", "string"},
            {"county", "string"},
            {"lat", "double"},
            {"lon", "double"}
        },
        1                  // primaryKeyField
    };

    HeaderBuffer::writeHeader(modifiedOutputFilename, header); // STEP 3: Write the header to the file

    // Append records with length-indicated lines
    ofstream appendFile(modifiedOutputFilename, ios::app);
    string line;
    getline(inputFile, line); // Skip CSV column header

    int recordCount = 0;
    while (getline(inputFile, line)) {
        int length = line.size();
        string lengthStr = (length < 10) ? "0" + to_string(length) : to_string(length);
        appendFile << lengthStr << "," << line << '\n';
        recordCount++;
    }

    appendFile.close();
    inputFile.close();

    // Update header with final record count
    header.recordCount = recordCount;
    HeaderBuffer::writeHeader(modifiedOutputFilename, header);

    cout << "Converted " << csvFilename << " to " << modifiedOutputFilename
         << " (length-indicated format)." << endl;
}
