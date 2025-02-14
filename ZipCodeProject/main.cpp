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

    std::ofstream outfile_txt("LocationSortedZips.txt");
    std::ofstream outfile_csv("LocationSortedZips.csv");
    if (!outfile_txt || !outfile_csv) {
        std::cerr << "Error: Unable to open output files." << std::endl;
        return 1;
    }

    // Write headers for both files
    outfile_txt << std::left << std::setw(5) << "State" << " | "
                << std::right << std::setw(12) << "Easternmost" << " | "
                << std::setw(12) << "Westernmost" << " | "
                << std::setw(12) << "Northernmost" << " | "
                << std::setw(12) << "Southernmost" << " |\n";
    outfile_txt << "===================================================================\n";

    outfile_csv << "State,Easternmost,Westernmost,Northernmost,Southernmost\n";

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

        outfile_txt << std::left << std::setw(5) << state << " | "
                    << std::right << std::setw(12) << east << " | "
                    << std::setw(12) << west << " | "
                    << std::setw(12) << north << " | "
                    << std::setw(12) << south << " |\n";

        outfile_csv << state << "," << east << "," << west << "," << north << "," << south << "\n";
    }

    outfile_txt.close();
    outfile_csv.close();
    std::cout << "Output written to LocationSortedZips.txt and LocationSortedZips.csv\n";
    return 0;
}
