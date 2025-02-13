#include <iostream>
#include <vector>
#include <algorithm>
#include "Buffer.h"

void processAndDisplayData(std::vector<ZipCodeRecord>& records, bool sortByPlaceName) {
    std::cout << "State | Easternmost Zip | Westernmost Zip | Northernmost Zip | Southernmost Zip\n";
    std::cout << "---------------------------------------------------------------\n";

    // Sort by Zip Code (Default) or Place Name based on user input
    if (sortByPlaceName) {
        std::sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.place_name < b.place_name;
        });
    } else {
        std::sort(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
            return a.zip_code < b.zip_code;
        });
    }

    std::string currentState = "";
    for (const auto& record : records) {
        if (record.state != currentState) {
            if (!currentState.empty()) std::cout << std::endl;
            currentState = record.state;
            std::cout << currentState << " | ";
        }
        std::cout << record.zip_code << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<ZipCodeRecord> records;
    Buffer buffer;

    if (!buffer.readCSV("us_postal_codes.csv", records)) {
        return 1;
    }

    // Default: Sort by Zip Code
    bool sortByPlaceName = false;

    // Check if an argument was provided
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "place") {
            sortByPlaceName = true;
        }
    }

    processAndDisplayData(records, sortByPlaceName);
    return 0;
}
