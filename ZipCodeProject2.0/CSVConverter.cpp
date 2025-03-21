/**
 * @file CSVConverter.cpp
 * @brief Implementation of CSVConverter for converting CSV to length-indicated format.
 */

#include "CSVConverter.h"
#include <iomanip>  // For setw() and setfill()
 
void CSVConverter::convertToLengthIndicated(const string& csvFilename, const string& outputFilename) {
    ifstream inputFile(csvFilename);

    string modifiedOutputFilename = outputFilename;
    if (modifiedOutputFilename.find(".txt") != string::npos) { // Ensure correct file extension (replace .txt with .csv if needed)
        modifiedOutputFilename.replace(modifiedOutputFilename.find(".txt"), 4, ".csv");
    }
 
    ofstream outputFile(modifiedOutputFilename);  // Open as text file
     
    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening files!" << endl;
        return;
    }
 
    // Write file header
    HeaderBuffer::FileHeader header = {"ZipCodeData", 1, 128, "ASCII", "index.txt", 0, 6};
    HeaderBuffer::writeHeader(modifiedOutputFilename, header);
 
    string line;
    getline(inputFile, line); // Skip CSV header row
     
    int recordCount = 0;
    while (getline(inputFile, line)) {
        int length = line.size();
         
        // Store length as a 2-digit ASCII number (makes sure that anything less than 10 characters is in the form 0x)
        string lengthStr = (length < 10) ? "0" + to_string(length) : to_string(length);
 
        // Write length as text, followed by the record
        outputFile << lengthStr << "," << line << '\n';
 
        recordCount++;
    }
 
    // Update record count in header
    header.recordCount = recordCount;
    HeaderBuffer::writeHeader(modifiedOutputFilename, header);
     
    // Close files
    inputFile.close();
    outputFile.close();
    cout << "Converted " << csvFilename << " to " << modifiedOutputFilename << " (length-indicated format)." << endl; // Completion message
}
 