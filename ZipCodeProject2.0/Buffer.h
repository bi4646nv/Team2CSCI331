/**
 * @file Buffer.h
 * @brief Header file for the Buffer class, which handles reading, processing, and validating zip code data.
 *
 * This class reads a length-indicated file containing zip codes, organizes the data by state,
 * allows sorting by Zip Code or Place Name, and verifies data integrity before processing.
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
 * @brief Structure to hold zip code data for a given location.
 */
struct ZipCodeRecord {
    int zip_code;         ///< zip code of the location.
    string place_name;    ///< City or place name.
    string state;         ///< Two-letter state abbreviation.
    string county;        ///< County name (can be empty).
    double lat;           ///< Latitude coordinate of the location.
    double lon;           ///< Longitude coordinate of the location.
};

/**
 * @class Buffer
 * @brief A class to handle reading, processing, and validating zip code data.
 */
class Buffer {
public:
    /**
     * @brief Reads a length-indicated file and stores zip code records.
     * @param filename The name of the length-indicated file to read.
     * @param records A vector to store the read zip code records.
     * @return True if the file is read successfully, false otherwise.
     *
     * This function reads a length-indicated file containing zip code data, extracts fields, checks for missing values,
     * and stores valid records into a vector.
     */
    bool readLengthIndicatedFile(const string& filename, vector<ZipCodeRecord>& records);

    /**
     * @brief Organizes zip code records by state.
     * @param records The vector of zip code records.
     * @param state_map A map to store zip codes categorized by state.
     *
     * This function groups zip code records by state into a map for easy retrieval.
     */
    void processRecords(const vector<ZipCodeRecord>& records, map<string, vector<ZipCodeRecord>>& state_map);
};

#endif // BUFFER_H
