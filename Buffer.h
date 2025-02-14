#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Structure to hold zip code data
struct ZipCodeRecord {
    int zip_code;
    std::string place_name;
    std::string state;
    std::string county;
    double lat;
    double lon;
};

class Buffer {
public:
    bool readCSV(const std::string& filename, std::vector<ZipCodeRecord>& records);
};

#endif // BUFFER_H
