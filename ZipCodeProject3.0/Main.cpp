/**
 * @file main.cpp
 * @brief Main program for testing the BSS file management
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BSSManager.h"

/**
 * @brief Print usage information
 */
void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./zipcode_bss create <csv_file> <data_file> <index_file> [block_size]" << std::endl;
    std::cout << "  ./zipcode_bss search <data_file> <index_file> -Z<zipcode>" << std::endl;
    std::cout << "  ./zipcode_bss insert <data_file> <index_file> <record_file>" << std::endl;
    std::cout << "  ./zipcode_bss delete <data_file> <index_file> <zipcode_file>" << std::endl;
    std::cout << "  ./zipcode_bss dump <data_file> <index_file> [physical|logical|index]" << std::endl;
}

/**
 * @brief Create a CSV file with sample data
 * @param filename Name of the CSV file
 */
void createSampleCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create sample CSV file " << filename << std::endl;
        return;
    }
    
    file << "ZipCode,City,State,County,Latitude,Longitude" << std::endl;
    file << "10001,New York,NY,New York,40.7128,-74.0060" << std::endl;
    file << "20001,Washington,DC,District of Columbia,38.9072,-77.0369" << std::endl;
    file << "30301,Atlanta,GA,Fulton,33.7490,-84.3880" << std::endl;
    file << "60601,Chicago,IL,Cook,41.8781,-87.6298" << std::endl;
    file << "75201,Dallas,TX,Dallas,32.7767,-96.7970" << std::endl;
    file << "90001,Los Angeles,CA,Los Angeles,34.0522,-118.2437" << std::endl;
    file << "94103,San Francisco,CA,San Francisco,37.7749,-122.4194" << std::endl;
    file << "98101,Seattle,WA,King,47.6062,-122.3321" << std::endl;
    file << "33101,Miami,FL,Miami-Dade,25.7617,-80.1918" << std::endl;
    file << "02108,Boston,MA,Suffolk,42.3601,-71.0589" << std::endl;
    
    file.close();
    std::cout << "Sample CSV file created: " << filename << std::endl;
}

/**
 * @brief Create a record file with sample data for insert test
 * @param filename Name of the record file
 */
void createSampleRecordFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create sample record file " << filename << std::endl;
        return;
    }
    
    file << "ZipCode,City,State,County,Latitude,Longitude" << std::endl;
    file << "12345,Testville,NY,Test County,40.0000,-74.0000" << std::endl;
    file << "54321,Sampleburg,CA,Sample County,37.0000,-120.0000" << std::endl;
    
    file.close();
    std::cout << "Sample record file created: " << filename << std::endl;
}

/**
 * @brief Create a zipcode file with sample data for delete test
 * @param filename Name of the zipcode file
 */
void createSampleZipcodeFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create sample zipcode file " << filename << std::endl;
        return;
    }
    
    file << "10001" << std::endl;
    file << "90001" << std::endl;
    
    file.close();
    std::cout << "Sample zipcode file created: " << filename << std::endl;
}

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char* argv[]) {
    // Generate sample files if no arguments are provided
    if (argc == 1) {
        std::cout << "Generating sample files for testing..." << std::endl;
        createSampleCSV("sample_zipcodes.csv");
        createSampleRecordFile("sample_records.csv");
        createSampleZipcodeFile("sample_zipcodes_to_delete.txt");
        printUsage();
        return 0;
    }
    
    // Otherwise, process command line arguments
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "create" && argc >= 5) {
        std::string csvFile = argv[2];
        std::string dataFile = argv[3];
        std::string indexFile = argv[4];
        int blockSize = (argc > 5) ? std::stoi(argv[5]) : 512;
        
        std::cout << "Creating BSS file from " << csvFile << "..." << std::endl;
        std::cout << "Data file: " << dataFile << std::endl;
        std::cout << "Index file: " << indexFile << std::endl;
        std::cout << "Block size: " << blockSize << " bytes" << std::endl;
        
        BSSManager manager(dataFile, indexFile);
        manager.initialize(blockSize);
        if (manager.createFromCSV(csvFile)) {
            std::cout << "BSS file created successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to create BSS file." << std::endl;
            return 1;
        }
    }
    else if (command == "search" && argc >= 4) {
        std::string dataFile = argv[2];
        std::string indexFile = argv[3];
        
        // Check for zip code flag
        std::string zipCode;
        for (int i = 4; i < argc; i++) {
            std::string arg = argv[i];
            if (arg.substr(0, 2) == "-Z") {
                zipCode = arg.substr(2);
            }
        }
        
        if (zipCode.empty()) {
            std::cerr << "Error: No zip code specified (-Z flag)" << std::endl;
            printUsage();
            return 1;
        }
        
        std::cout << "Searching for zip code " << zipCode << "..." << std::endl;
        
        BSSManager manager(dataFile, indexFile);
        ZipCodeRecord record;
        if (manager.search(zipCode, record)) {
            std::cout << "Found:" << std::endl;
            std::cout << "Zip Code: " << record.getZipCode() << std::endl;
            std::cout << "City: " << record.getCityName() << std::endl;
            std::cout << "State: " << record.getStateName() << std::endl;
            std::cout << "County: " << record.getCountyName() << std::endl;
            std::cout << "Latitude: " << record.getLatitude() << std::endl;
            std::cout << "Longitude: " << record.getLongitude() << std::endl;
            return 0;
        } else {
            std::cout << "Zip code " << zipCode << " not found." << std::endl;
            return 1;
        }
    }
    else if (command == "insert" && argc >= 5) {
        std::string dataFile = argv[2];
        std::string indexFile = argv[3];
        std::string recordFile = argv[4];
        
        std::cout << "Inserting records from " << recordFile << "..." << std::endl;
        
        // Read records from file
        std::ifstream file(recordFile);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open record file " << recordFile << std::endl;
            return 1;
        }
        
        std::string line;
        std::getline(file, line);  // Skip header line
        
        BSSManager manager(dataFile, indexFile);
        int count = 0;
        
        while (std::getline(file, line)) {
            ZipCodeRecord record = ZipCodeRecord::fromCSV(line);
            if (manager.insert(record)) {
                std::cout << "Inserted: " << record.getZipCode() << std::endl;
                count++;
            } else {
                std::cerr << "Failed to insert: " << record.getZipCode() << std::endl;
            }
        }
        
        file.close();
        std::cout << "Inserted " << count << " records." << std::endl;
        return 0;
    }
    else if (command == "delete" && argc >= 5) {
        std::string dataFile = argv[2];
        std::string indexFile = argv[3];
        std::string zipcodeFile = argv[4];
        
        std::cout << "Deleting records from " << zipcodeFile << "..." << std::endl;
        
        // Read zip codes from file
        std::ifstream file(zipcodeFile);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open zipcode file " << zipcodeFile << std::endl;
            return 1;
        }
        
        std::string line;
        BSSManager manager(dataFile, indexFile);
        int count = 0;
        
        while (std::getline(file, line)) {
            if (manager.remove(line)) {
                std::cout << "Deleted: " << line << std::endl;
                count++;
            } else {
                std::cerr << "Failed to delete: " << line << std::endl;
            }
        }
        
        file.close();
        std::cout << "Deleted " << count << " records." << std::endl;
        return 0;
    }
    else if (command == "dump" && argc >= 4) {
        std::string dataFile = argv[2];
        std::string indexFile = argv[3];
        std::string dumpType = (argc > 4) ? argv[4] : "physical";
        
        BSSManager manager(dataFile, indexFile);
        
        if (dumpType == "physical") {
            std::cout << "Physical dump:" << std::endl;
            manager.dumpPhysical();
        }
        else if (dumpType == "logical") {
            std::cout << "Logical dump:" << std::endl;
            manager.dumpLogical();
        }
        else if (dumpType == "index") {
            std::cout << "Index dump:" << std::endl;
            manager.dumpIndex();
        }
        else {
            std::cerr << "Error: Unknown dump type " << dumpType << std::endl;
            printUsage();
            return 1;
        }
        
        return 0;
    }
    else {
        printUsage();
        return 1;
    }
}