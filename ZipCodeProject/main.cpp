#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include "Buffer.h"

int main() {
    std::vector<ZipCodeRecord> records;
    Buffer buffer;
    std::map<std::string, std::vector<ZipCodeRecord>> state_map;
    std::string filename = "us_postal_codes.csv";

    if (!buffer.readCSV(filename, records)) {
        std::cerr << "Error: Unable to read CSV file." << std::endl;
        return 1;
    }

    buffer.processRecords(records, state_map);

    std::ofstream outfile("LocationSortedZips.txt");
    if (!outfile) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }

    outfile << std::left << std::setw(5) << "State" << " | "
            << std::right << std::setw(12) << "Easternmost" << " | "
            << std::setw(12) << "Westernmost" << " | "
            << std::setw(12) << "Northernmost" << " | "
            << std::setw(12) << "Southernmost" << " |\n";
    outfile << "===================================================================\n";

    for (const auto& entry : state_map) {
        const std::string& state = entry.first;
        const std::vector<ZipCodeRecord>& zipRecords = entry.second;

        int east, west, north, south;
        double minLon = std::numeric_limits<double>::max();
        double maxLon = std::numeric_limits<double>::lowest();
        double maxLat = std::numeric_limits<double>::lowest();
        double minLat = std::numeric_limits<double>::max();

        for (const auto& record : zipRecords) {
            if (record.lon < minLon) { minLon = record.lon; east = record.zip_code; }
            if (record.lon > maxLon) { maxLon = record.lon; west = record.zip_code; }
            if (record.lat > maxLat) { maxLat = record.lat; north = record.zip_code; }
            if (record.lat < minLat) { minLat = record.lat; south = record.zip_code; }
        }

        outfile << std::left << std::setw(5) << state << " | "
                << std::right << std::setw(12) << east << " | "
                << std::setw(12) << west << " | "
                << std::setw(12) << north << " | "
                << std::setw(12) << south << " |\n";
    }

    outfile.close();
    std::cout << "Output written to LocationSortedZips.txt\n";
    return 0;
}