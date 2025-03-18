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

    string line;
    while (getline(file, line)) {
        if (line.size() < 3) {  // Ensure valid line
            cerr << "Error: Malformed record: " << line << endl;
            continue;
        }

        // Extract first two characters as length
        string lengthStr = line.substr(0, 2);
        int recordLength = stoi(lengthStr); // Convert to integer

        // Extract actual CSV record after the length
        string recordData = line.substr(3);

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
        } catch (const exception& e) {
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
