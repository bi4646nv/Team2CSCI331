#include <iostream>
#include <vector>
#include "Buffer.h"

int main() {
    std::vector<ZipCodeRecord> records;
    Buffer buffer;
    
    std::string filename = "us_postal_codes.csv";
    
    if (!buffer.readCSV(filename, records)) {
        std::cerr << "Error: Unable to read CSV file." << std::endl;
        return 1;
    }

    std::cout << "CSV File Read Successfully! Total Records: " << records.size() << std::endl;

    for (const auto& record : records) {
        std::cout << record.zip_code << " | " << record.place_name << " | " << record.state 
                  << " | " << record.county << " | " << record.lat << " | " << record.lon << std::endl;
    }

    return 0;
}
