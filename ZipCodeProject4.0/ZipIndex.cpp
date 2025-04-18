#include "ZipIndex.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

/**
 * @brief Builds the index from a length-indicated file.
 * @param filename The file containing length-indicated Zip Code records.
 */
void ZipIndex::buildIndex(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << " Error opening file: " << filename << endl;
        return;
    }

    index.clear();
    string line;

    // Skip header
    if (!getline(file, line)) {
        cerr << "Error: CSV file is empty or not formatted correctly!" << endl;
        return;
    }
    cout << " Skipping header: " << line << endl;

    // Store the current file position (beginning of data)
    long offset = file.tellg();

    while (getline(file, line)) {
        if (line.empty()) continue; // Ignore empty lines

        // Store current offset for this line
        long currentOffset = offset;
        
        // Update offset for next line
        offset = file.tellg();

        stringstream ss(line);
        string recordLength, zipCode;

        //  Skip the first column (record length)
        if (!getline(ss, recordLength, ',')) continue;
        
        // ✅ Extract the second column (actual Zip Code)
        if (!getline(ss, zipCode, ',')) continue;

        // ✅ Ensure zip code is stored as exactly 5 digits
        while (zipCode.length() < 5) {
            zipCode = "0" + zipCode;
        }

        // ✅ Validate Zip Code (must be numeric and 5 digits long)
        if (zipCode.length() != 5 || zipCode.find_first_not_of("0123456789") != string::npos) {
            cerr << " Warning: Invalid zip code in line -> " << line << endl;
            continue;
        }

        // ✅ Store offset for this Zip Code
        index[zipCode] = currentOffset;
        cout << "✅ Indexed Zip Code: " << zipCode << " at offset " << currentOffset << endl;
    }

    file.close();
    cout << "✅ Indexing complete. Total entries: " << index.size() << endl;
}

/**
 * @brief Saves the index to a file.
 * @param indexFilename The output index file.
 */
void ZipIndex::saveIndex(const std::string& indexFilename) {
    ofstream file(indexFilename, ios::out | ios::trunc);
    if (!file) {
        cerr << " Error opening index file: " << indexFilename << endl;
        return;
    }

    for (const auto& entry : index) {
        file << entry.first << "," << entry.second << "\n";
    }

    file.close();
    cout << "Index successfully saved to " << indexFilename << endl;
}

/**
 * @brief Loads the index from a file.
 * @param indexFilename The index file to load.
 */
void ZipIndex::loadIndex(const std::string& indexFilename) {
    ifstream file(indexFilename);
    if (!file.is_open()) {
        cerr << " Error opening index file: " << indexFilename << endl;
        return;
    }

    index.clear();
    string line;
    string zipCode;
    long offset;
    
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, zipCode, ',');

        if (ss.good()) {
            string offsetStr;
            getline(ss, offsetStr);
            try {
                offset = stol(offsetStr);
                index[zipCode] = offset;
            } catch (const std::exception& e) {
                cerr << " Warning: Invalid offset value in line: " << line << endl;
            }
        }
    }
    file.close();
    
    cout << " Index loaded successfully with " << index.size() << " entries." << endl;
}

/**
 * @brief Finds the byte offset of a given Zip Code.
 * @param zipCode The Zip Code to search for.
 * @return The byte offset if found, or -1 if not found.
 */
long ZipIndex::findZipCode(const std::string& zipCode) const {
    string formattedZip = zipCode;
    while (formattedZip.length() < 5) {
        formattedZip = "0" + formattedZip;
    }
    
    auto it = index.find(formattedZip);
    if (it != index.end()) {
        return it->second;
    }
    return -1;
}
