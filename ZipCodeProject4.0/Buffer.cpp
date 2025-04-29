/**
 * @file Buffer.cpp
 * @brief Implementation of the Buffer class for handling zip code data processing and validation.
 */

#include "Buffer.h"

using namespace std;

bool Buffer::readLengthIndicatedFile(const string& filename, vector<ZipCodeRecord>& records) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the length-indicated file " << filename << endl;
        return false;
    }

    int linesToSkip = 15; // Skip the 15 header lines: 8 fixed + 6 fields + 1 primary key line
    string dummy;
    for (int i = 0; i < linesToSkip; ++i) {
        getline(file, dummy);
    }

    string line;
    while (getline(file, line)) {
        if (line.size() < 3) {  // Check for valid line format
            cerr << "Error: Malformed record: " << line << endl;
            continue;
        }
        
        string lengthStr = line.substr(0, 2); // Extract first two characters as length
        int recordLength = stoi(lengthStr); // Convert to length value integer

        string recordData = line.substr(3); // Extract the actual CSV record after the length is processed

        stringstream ss(recordData);
        ZipCodeRecord record;
        string zip, lat, lon;
        vector<string> values;
        string token;

        while (getline(ss, token, ',')) {
            values.push_back(token);
        }

        if (values.size() != 6) {
            cerr << "Error: Incorrect number of fields in length-indicated record: " << recordData << endl;
            continue;
        }

        // Fill ZipCodeRecord fields
        zip = values[0];
        record.place_name = values[1];
        record.state = values[2];
        record.county = values[3];
        lat = values[4];
        lon = values[5];

        try {
            record.zip_code = stoi(zip);
            record.lat = stod(lat);
            record.lon = stod(lon);
        } catch (const exception& e) { // Error catching
            cerr << "Error parsing numeric values in record: " << recordData << " - " << e.what() << endl;
            continue;
        }

        records.push_back(record);
    }

    file.close();
    return true;
}

void Buffer::processRecords(const vector<ZipCodeRecord>& records, map<string, vector<ZipCodeRecord>>& state_map) {
    for (const auto& record : records) {
        state_map[record.state].push_back(record);
    }
}
