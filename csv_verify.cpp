#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Buffer.h"

bool verifyCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Read the header line
    
    if (line.empty()) {
        std::cerr << "Error: CSV file is empty or missing headers." << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string zip, place_name, state, county, lat, lon;
        
        if (!std::getline(ss, zip, ',') ||
            !std::getline(ss, place_name, ',') ||
            !std::getline(ss, state, ',') ||
            !std::getline(ss, county, ',') ||
            !std::getline(ss, lat, ',') ||
            !std::getline(ss, lon, ',')) {
            std::cerr << "Error: Malformed line - " << line << std::endl;
            return false;
        }

        if (state.length() != 2) {
            std::cerr << "Error: Invalid state abbreviation in line - " << line << std::endl;
            return false;
        }
    }

    std::cout << "CSV file verification completed successfully!" << std::endl;
    return true;
}

int main() {
    std::string filename = "us_postal_codes.csv";
    verifyCSV(filename);
    return 0;
}
