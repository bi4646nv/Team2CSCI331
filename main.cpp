#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "buffer.h"

// Function to display the CSV file in a table format
void displayCSV(const std::vector<ZipCodeRecord>& records) {
    std::cout << "\"Zip Code\" | \"Place Name\" | State | County | Lat | Long |" << std::endl;
    for (const auto& record : records) {
        std::cout << record.zip_code << " | " 
                  << record.place_name << " | " 
                  << record.state << " | " 
                  << record.county << " | " 
                  << record.lat << " | " 
                  << record.lng << " | " 
                  << std::endl;
    }
}

int main() {
    std::string input_file = "us_postal_codes.csv";
    std::string output_file = "formatted_zip_codes.txt";
    
    // Create a buffer object and use it to read the CSV
    Buffer buffer;
    std::vector<ZipCodeRecord> records;
    if (buffer.readCSV(input_file, records)) {
        // Display the records read from the file
        displayCSV(records);
        
        // Process and write to output file (if necessary)
        std::ofstream out(output_file);
        if (out.is_open()) {
            out << "\"Zip Code\" | \"Place Name\" | State | County | Lat | Long |" << std::endl;
            for (const auto& record : records) {
                out << record.zip_code << " | " 
                    << record.place_name << " | " 
                    << record.state << " | " 
                    << record.county << " | " 
                    << record.lat << " | " 
                    << record.lng << " | " 
                    << std::endl;
            }
            out.close();
            std::cout << "Formatted data has been written to " << output_file << std::endl;
        } else {
            std::cerr << "Error opening output file for writing." << std::endl;
        }
    } else {
        std::cerr << "Error reading the input CSV file." << std::endl;
    }
    
    return 0;
}
