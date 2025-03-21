/**
 * @file main.cpp
 * @brief Main program for processing zip code data and generating tables.
 *
 * This program reads zip code data from a chosen length-indicated file, sorts it based 
 * on user selection (Zip Code or Place Name), and then organizes the data by state.
 * It outputs the easternmost (least longitude), westernmost, northernmost 
 * (greatest latitude), and southernmost Zip Code in that state, in that order.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include "Buffer.h"
#include "HeaderBuffer.h"

using namespace std;

/**
 * @brief Main function to process zip code data.
 * @return 0 on successful execution, -1 on error.
 */
int main() {
    vector<ZipCodeRecord> records; ///< Holds all zip code records read from file
    Buffer buffer; ///< Instance of Buffer to process records
    map<string, vector<ZipCodeRecord>> state_map; ///< Map to store zip code records categorized by state

    string filename;
    int fileChoice;

    while (true) { // Ask user for file selection
        cout << "Select data file:\n";
        cout << "1 - us_postal_codes_length.csv\n";
        cout << "2 - us_postal_codes_random_length.csv\n";
        cout << "Enter choice (1 or 2): ";
        cin >> fileChoice;

        if (fileChoice == 1) {
            filename = "us_postal_codes_length.csv";  // Selecting sorted CSV file
            break;
        } else if (fileChoice == 2) {
            filename = "us_postal_codes_random_length.csv";  // Selecting randomized (not sorted) CSV file
            break;
        } else {
            cout << "Invalid choice! Please enter 1 or 2.\n"; // Continue until user selects proper option
        }
    }

    HeaderBuffer::FileHeader header = HeaderBuffer::readHeader(filename); // Read file header

    
    if (!buffer.readLengthIndicatedFile(filename, records)) { // Read length-indicated file
        cerr << "Error: Unable to read length-indicated file: " << filename << endl;
        return -1;
    }

    char sortChoice;
    while (true) { // Ask user for sorting preference
        cout << "Do you want to sort by Zip Code (Z) or Place Name (P): ";
        cin >> sortChoice;
        sortChoice = toupper(sortChoice);

        if (sortChoice == 'Z' || sortChoice == 'P') {
            break;
        }
        cout << "Invalid choice! Please enter 'Z' for Zip Code or 'P' for Place Name.\n";
    }

    if (sortChoice == 'Z') { // Sort data based on user choice
        sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.zip_code < b.zip_code;
        });
    } else {
        sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.place_name < b.place_name;
        });
    }

    buffer.processRecords(records, state_map);

    ofstream outfile_txt("SortedLocations.txt"); ///< Output file for readable table
    ofstream outfile_csv("SortedLocations.csv"); ///< Output file for CSV (^ same information as table)

    // Adjust column widths for output formatting
    int stateWidth = 5;
    int fieldWidth = (sortChoice == 'Z') ? 12 : 20;

    // Print headers
    outfile_txt << left << setw(stateWidth) << "State" << " | "
                << setw(fieldWidth) << "Easternmost" << " | "
                << setw(fieldWidth) << "Westernmost" << " | "
                << setw(fieldWidth) << "Northernmost" << " | "
                << setw(fieldWidth) << "Southernmost" << " |\n";

    outfile_txt << string((6 + (fieldWidth + 3) * 4) + 1, '=') << "\n";

    outfile_csv << "State,Easternmost,Westernmost,Northernmost,Southernmost\n";

    // Process each state and determine furthest locations
    for (const auto& entry : state_map) {
        const string& state = entry.first;
        const vector<ZipCodeRecord>& zipRecords = entry.second;

        string eastPlace, westPlace, northPlace, southPlace;
        int eastZip, westZip, northZip, southZip;
        double minLon = numeric_limits<double>::max();
        double maxLon = numeric_limits<double>::lowest();
        double maxLat = numeric_limits<double>::lowest();
        double minLat = numeric_limits<double>::max();
        
        // Find furthest zip codes based on coordinates
        for (const auto& record : zipRecords) {
            if (record.lon < minLon) {
                minLon = record.lon;
                eastZip = record.zip_code;
                eastPlace = record.place_name;
            }
            if (record.lon > maxLon) {
                maxLon = record.lon;
                westZip = record.zip_code;
                westPlace = record.place_name;
            }
            if (record.lat > maxLat) {
                maxLat = record.lat;
                northZip = record.zip_code;
                northPlace = record.place_name;
            }
            if (record.lat < minLat) {
                minLat = record.lat;
                southZip = record.zip_code;
                southPlace = record.place_name;
            }
        }
        
        // Write results to output files
        if (sortChoice == 'Z') {
            outfile_txt << left << setw(stateWidth) << state << " | "
                        << right << setw(fieldWidth) << eastZip << " | "
                        << setw(fieldWidth) << westZip << " | "
                        << setw(fieldWidth) << northZip << " | "
                        << setw(fieldWidth) << southZip << " |\n";

            outfile_csv << state << "," << eastZip << "," << westZip << "," << northZip << "," << southZip << "\n";
        }
    }

    // Close output files
    outfile_txt.close();
    outfile_csv.close();
    cout << "Processing complete. Results written to SortedLocations.txt and SortedLocations.csv." << endl; // Completion message
    return 0;
}
