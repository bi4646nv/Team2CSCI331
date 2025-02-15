/**
 * @file Buffer.h
 * @brief Header file for the Buffer class, which handles reading, processing, and validating ZIP code data.
 *
 * This class reads a CSV file containing ZIP codes, organizes the data by state,
 * and verifies data integrity before processing.
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <map>

using namespace std;

/**
 * @struct ZipCodeRecord
 * @brief Structure to hold ZIP code data.
 */
struct ZipCodeRecord {
    int zip_code;         ///< ZIP code
    string place_name;    ///< City or place name
    string state;         ///< Two-letter state abbreviation
    string county;        ///< County name
    double lat;           ///< Latitude
    double lon;           ///< Longitude
};

/**
 * @class Buffer
 * @brief A class to handle reading, processing, and validating ZIP code data.
 */
class Buffer {
public:
    /**
     * @brief Reads a CSV file, validates, and stores ZIP code records.
     * @param filename The name of the CSV file to read.
     * @param records Vector to store the read ZIP code records.
     * @return True if the file is read successfully, false otherwise.
     */
    bool readCSV(const string& filename, vector<ZipCodeRecord>& records);

    /**
     * @brief Organizes ZIP code records by state.
     * @param records The vector of ZIP code records.
     * @param state_map Map to store ZIP codes categorized by state.
     */
    void processRecords(const vector<ZipCodeRecord>& records, map<string, vector<ZipCodeRecord>>& state_map);

private:
    /**
     * @brief Verifies the integrity of the CSV file and prints missing values.
     * @param filename The name of the CSV file to check.
     */
    void verifyCSVIntegrity(const string& filename);
};

#endif // BUFFER_H