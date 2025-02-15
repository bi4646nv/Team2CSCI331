/**
 * @file main.cpp
 * @brief Main program for processing ZIP code data and generating reports.
 *
 * This program reads ZIP code data from a CSV file (default: us_postal_codes.csv),
 * but allows the user to enter a different file name if desired. It organizes the data
 * by state and outputs the easternmost, westernmost, northernmost, and southernmost
 * ZIP codes for each state to a text file and a CSV file.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include "Buffer.h"

using namespace std;

/**
 * @brief Main function to process ZIP code data.
 *
 * The program first prompts the user to either use the default input file
 * or enter a custom file name. It then reads ZIP code records from the chosen file,
 * processes the records by state, and outputs the results to text and CSV files.
 *
 * @return 0 on successful execution, -1 on error.
 */
int main() {
    vector<ZipCodeRecord> records;
    Buffer buffer;
    map<string, vector<ZipCodeRecord>> state_map;

    // Default file
    string filename = "us_postal_codes.csv";

    // Ask user if they want to change input file
    cout << "Default file input: \"" << filename << "\"\n";
    cout << "Would you like to change this? (Y/N): ";
    char choice;
    cin >> choice;

    if (toupper(choice) == 'Y') {
        cout << "Enter the input file name: ";
        cin >> filename;
    }

    // Read the CSV file
    if (!buffer.readCSV(filename, records)) {
        cerr << "Error: Unable to read CSV file: " << filename << endl;
        return -1;
    }

    // Process ZIP code records and organize them by state
    buffer.processRecords(records, state_map);

    // Open output files
    ofstream outfile_txt("LocationSortedZips.txt");
    ofstream outfile_csv("LocationSortedZips.csv");
    if (!outfile_txt || !outfile_csv) {
        cerr << "Error: Unable to open output files." << endl;
        return -1;
    }

    // Write headers for both files
    outfile_txt << left << setw(5) << "State" << " | "
                << right << setw(12) << "Easternmost" << " | "
                << setw(12) << "Westernmost" << " | "
                << setw(12) << "Northernmost" << " | "
                << setw(12) << "Southernmost" << " |\n";
    outfile_txt << "===================================================================\n";

    outfile_csv << "State,Easternmost,Westernmost,Northernmost,Southernmost\n";

    // Process each state and find extreme ZIP codes
    for (const auto& entry : state_map) {
        const string& state = entry.first;
        const vector<ZipCodeRecord>& zipRecords = entry.second;

        int east, west, north, south;
        double minLon = numeric_limits<double>::max();
        double maxLon = numeric_limits<double>::lowest();
        double maxLat = numeric_limits<double>::lowest();
        double minLat = numeric_limits<double>::max();

        for (const auto& record : zipRecords) {
            if (record.lon < minLon) { minLon = record.lon; east = record.zip_code; }
            if (record.lon > maxLon) { maxLon = record.lon; west = record.zip_code; }
            if (record.lat > maxLat) { maxLat = record.lat; north = record.zip_code; }
            if (record.lat < minLat) { minLat = record.lat; south = record.zip_code; }
        }

        outfile_txt << left << setw(5) << state << " | "
                    << right << setw(12) << east << " | "
                    << setw(12) << west << " | "
                    << setw(12) << north << " | "
                    << setw(12) << south << " |\n";

        outfile_csv << state << "," << east << "," << west << "," << north << "," << south << "\n";
    }

    // Close files and finish execution
    outfile_txt.close();
    outfile_csv.close();
    cout << "Output written to LocationSortedZips.txt and LocationSortedZips.csv\n";
    return 0;
}
