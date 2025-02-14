#include "Buffer.h"

bool Buffer::readCSV(const std::string& filename, std::vector<ZipCodeRecord>& records) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ZipCodeRecord record;
        std::string zip, lat, lon;
        
        std::getline(ss, zip, ',');
        std::getline(ss, record.place_name, ',');
        std::getline(ss, record.state, ',');
        std::getline(ss, record.county, ',');
        std::getline(ss, lat, ',');
        std::getline(ss, lon, ',');
        
        try {
            record.zip_code = std::stoi(zip);
            record.lat = std::stod(lat);
            record.lon = std::stod(lon);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
            continue;
        }
        
        records.push_back(record);
    }

    file.close();
    return true;
}

void Buffer::processRecords(const std::vector<ZipCodeRecord>& records, std::map<std::string, std::vector<ZipCodeRecord>>& state_map) {
    for (const auto& record : records) {
        state_map[record.state].push_back(record);
    }
}
