#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct ZipCodeRecord {
    string zipCode;
    string placeName;
    string state;
    string county;
    string latitude;
    string longitude;
};

// Function to verify the integrity of the CSV file
bool verifyCSVIntegrity(const string& filename) {
    ifstream file(filename);
    string line;
    vector<string> headers;
    bool isFirstLine = true;

    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return false;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> values;
        
        while (getline(ss, token, ',')) {
            values.push_back(token);
        }

        if (isFirstLine) {
            // Store headers and verify them
            headers = values;
            isFirstLine = false;

            if (headers[0] != "\"Zip Code\"" || headers[1] != "\"Place Name\"" || headers[2] != "State" || 
                headers[3] != "County" || headers[4] != "Lat" || headers[5] != "Long") {
                cerr << "Error: Incorrect headers in CSV file." << endl;
                return false;
            }

            continue; // Skip further validation for the header row
        }

        // Check for missing values (if the row has fewer than expected columns)
        if (values.size() != 6) {
            cerr << "Error: Incorrect number of columns on line: " << line << endl;
            return false;
        }

        // Check for missing or incorrect values in each column
        if (values[0].empty() || values[2].empty() || values[3].empty() || values[4].empty() || values[5].empty()) {
            cerr << "Error: Missing values on line: " << line << endl;
            return false;
        }

        // Check if the state is exactly two characters long
        if (values[2].length() != 2) {
            cerr << "Error: Invalid state on line: " << line << endl;
            return false;
        }

        // Additional validation checks can go here (e.g., numeric Zip Codes)
    }

    file.close();
    return true;
}

int main() {
    string filename = "us_postal_codes.csv"; // Specify the path to your CSV file
    if (verifyCSVIntegrity(filename)) {
        cout << "CSV Integrity verified successfully!" << endl;
    } else {
        cout << "CSV Integrity check failed." << endl;
    }

    return 0;
}

