/**
 * @file main.cpp
 * @brief Main program for processing zip code data and generating reports.
 *
 * This program reads zip code data from a CSV file (us_postal_codes.csv),
 * sorts it based on user selection (Zip Code or Place Name), and then organizes
 * the data by state. It outputs the easternmost, westernmost, northernmost,
 * and southernmost locations using either Zip Codes or Place Names.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include "Buffer.h"

using namespace std;

/**
 * @brief Main function to process zip code data.
 * @return 0 on successful execution, -1 on error.
 */
int main() {
    vector<ZipCodeRecord> records;
    Buffer buffer;
    map<string, vector<ZipCodeRecord>> state_map;

    string filename = "us_postal_codes.csv";

    // Ask user for sorting preference
    char sortChoice;
    while (true) {
        cout << "Do you want to sort by Zip Code (Z) or Place Name (P): ";
        cin >> sortChoice;
        sortChoice = toupper(sortChoice);

        if (sortChoice == 'Z' || sortChoice == 'P') {
            break;
        }
        cout << "Invalid choice! Please enter 'Z' for Zip Code or 'P' for Place Name.\n";
    }

    // Read CSV file
    if (!buffer.readCSV(filename, records)) {
        cerr << "Error: Unable to read CSV file: " << filename << endl;
        return -1;
    }

    // Sort data based on user choice
    if (sortChoice == 'Z') {
        sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.zip_code < b.zip_code;
        });
    } else {
        sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.place_name < b.place_name;
        });
    }

    buffer.processRecords(records, state_map);

    ofstream outfile_txt("SortedLocations.txt");
    ofstream outfile_csv("SortedLocations.csv");

    // Adjust column widths
    int stateWidth = 5;
    int fieldWidth = (sortChoice == 'Z') ? 12 : 20;  // Zip codes = 12 width, Place names = 20 width

    // Print headers
    outfile_txt << left << setw(stateWidth) << "State" << " | "
                << setw(fieldWidth) << "Easternmost" << " | "
                << setw(fieldWidth) << "Westernmost" << " | "
                << setw(fieldWidth) << "Northernmost" << " | "
                << setw(fieldWidth) << "Southernmost" << " |\n";

    outfile_txt << string((6 + (fieldWidth + 3) * 4) + 1, '=') << "\n";

    outfile_csv << "State,Easternmost,Westernmost,Northernmost,Southernmost\n";

    // Process each state and determine extreme locations
    for (const auto& entry : state_map) {
        const string& state = entry.first;
        const vector<ZipCodeRecord>& zipRecords = entry.second;

        string eastPlace, westPlace, northPlace, southPlace;
        int eastZip, westZip, northZip, southZip;
        double minLon = numeric_limits<double>::max();
        double maxLon = numeric_limits<double>::lowest();
        double maxLat = numeric_limits<double>::lowest();
        double minLat = numeric_limits<double>::max();

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

        if (sortChoice == 'Z') {
            // Output using zip Codes
            outfile_txt << left << setw(stateWidth) << state << " | "
                        << right << setw(fieldWidth) << eastZip << " | "
                        << setw(fieldWidth) << westZip << " | "
                        << setw(fieldWidth) << northZip << " | "
                        << setw(fieldWidth) << southZip << " |\n";

            outfile_csv << state << "," << eastZip << "," << westZip << "," << northZip << "," << southZip << "\n";
        } else {
            // Output using Place Names
            outfile_txt << left << setw(stateWidth) << state << " | "
                        << left << setw(fieldWidth) << eastPlace << " | "
                        << left << setw(fieldWidth) << westPlace << " | "
                        << left << setw(fieldWidth) << northPlace << " | "
                        << left << setw(fieldWidth) << southPlace << " |\n";

            outfile_csv << state << "," << eastPlace << "," << westPlace << "," << northPlace << "," << southPlace << "\n";
        }
    }

    // Close files
    outfile_txt.close();
    outfile_csv.close();
    cout << "Output written to SortedLocations.txt and SortedLocations.csv\n";
    return 0;
}
