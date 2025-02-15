/**
 * @file Buffer.cpp
 * @brief Implementation of the Buffer class for handling ZIP code data processing and validation.
 *
 * This file contains the implementation of methods that read a CSV file,
 * parse ZIP code records, validate their integrity, and categorize them by state.
 */

#include "Buffer.h"

using namespace std;

/**
 * @brief Reads a CSV file, validates data integrity, and stores ZIP code records.
 * @param filename The name of the CSV file to read.
 * @param records Vector to store the read ZIP code records.
 * @return True if the file is read successfully, false otherwise.
 */
bool Buffer::readCSV(const string& filename, vector<ZipCodeRecord>& records) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        ZipCodeRecord record;
        string zip, lat, lon;
        vector<string> values;
        string token;

        while (getline(ss, token, ',')) {
            values.push_back(token);
        }

        // Ensure we have all expected columns
        if (values.size() != 6) {
            cerr << "Error: Incorrect number of columns on line: " << line << endl;
            continue; // Skip this entry, but do not terminate program
        }

        // Extract values safely
        zip = values[0];
        record.place_name = values[1];
        record.state = values[2];
        record.county = values[3]; // County may be empty
        lat = values[4];
        lon = values[5];

        // Print warnings for missing non-critical fields
        if (record.county.empty()) {
            cerr << "Warning: Missing county on line: " << line << endl;
        }

        // Validate critical fields
        if (zip.empty() || record.state.empty() || lat.empty() || lon.empty()) {
            cerr << "Error: Missing critical values on line: " << line << endl;
            continue; // Skip entry, but keep processing
        }

        // Ensure state is two characters
        if (record.state.length() != 2) {
            cerr << "Error: Invalid state format on line: " << line << endl;
            continue;
        }

        try {
            record.zip_code = stoi(zip);
            record.lat = stod(lat);
            record.lon = stod(lon);
        } catch (const exception& e) {
            cerr << "Error parsing numeric values on line: " << line << " - " << e.what() << endl;
            continue;
        }

        records.push_back(record);
    }

    file.close();
    return true;
}

/**
 * @brief Organizes ZIP code records by state.
 * @param records The vector of ZIP code records.
 * @param state_map Map to store ZIP codes categorized by state.
 */
void Buffer::processRecords(const vector<ZipCodeRecord>& records, map<string, vector<ZipCodeRecord>>& state_map) {
    for (const auto& record : records) {
        state_map[record.state].push_back(record);
    }
}

/**
 * @brief Verifies the integrity of the CSV file and prints missing values.
 * @param filename The name of the CSV file to check.
 */
void Buffer::verifyCSVIntegrity(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> values;
        string token;

        while (getline(ss, token, ',')) {
            values.push_back(token);
        }

        // Ensure we have all expected columns
        if (values.size() != 6) {
            cerr << "Error: Incorrect number of columns on line: " << line << endl;
            continue; // Skip this entry, but do not terminate program
        }

        // Validate critical fields
        if (values[0].empty() || values[2].empty() || values[4].empty() || values[5].empty()) {
            cerr << "Error: Missing critical values on line: " << line << endl;
        }
    }

    file.close();
}
