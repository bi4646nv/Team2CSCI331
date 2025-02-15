/**
 * @file Buffer.cpp
 * @brief Implementation of the Buffer class for handling zip code data processing and validation.
 */

 #include "Buffer.h"

 using namespace std;
 
 bool Buffer::readCSV(const string& filename, vector<ZipCodeRecord>& records) {
     ifstream file(filename);
     if (!file.is_open()) {
         cerr << "Error: Could not open the file " << filename << endl;
         return false;
     }
 
     string line;
     getline(file, line); // Skip header
 
     while (getline(file, line)) {
         stringstream ss(line);
         ZipCodeRecord record;
         string zip, lat, lon;
         vector<string> values;
         string token;
 
         while (getline(ss, token, ',')) {
             values.push_back(token);
         }
 
         if (values.size() != 6) {
             cerr << "Error: Incorrect number of columns on line: " << line << endl;
             continue;
         }
 
         zip = values[0];
         record.place_name = values[1];
         record.state = values[2];
         record.county = values[3];
         lat = values[4];
         lon = values[5];
 
         if (record.county.empty()) {
             cerr << "Warning: Missing county on line: " << line << endl;
         }
 
         if (zip.empty() || record.state.empty() || lat.empty() || lon.empty()) {
             cerr << "Error: Missing critical values on line: " << line << endl;
             continue;
         }
 
         try {
             record.zip_code = stoi(zip);
             record.lat = stod(lat);
             record.lon = stod(lon);
         } catch (const exception& e) {
             cerr << "Error parsing numeric values on line: " << line << " - " << e.what() << endl;
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
 