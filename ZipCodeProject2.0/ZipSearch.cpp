#include <iostream>
#include <fstream>
#include "ZipIndex.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <datafile> <indexfile> -Z<zip>\n";
        return 1;
    }

    string dataFilename = argv[1];
    string indexFilename = argv[2];
    string zipCode;

    for (int i = 3; i < argc; ++i) {
        string arg = argv[i];
        if (arg.size() > 2 && arg[0] == '-' && arg[1] == 'Z') {
            zipCode = arg.substr(2);
            break;
        }
    }

    if (zipCode.empty()) {
        cerr << " Error: No valid ZIP code provided for search.\n";
        return 1;
    }

    cout << "🔍 Searching for ZIP code: " << zipCode << endl;

    ZipIndex index;
    cout << "📥 Loading index from " << indexFilename << endl;
    index.loadIndex(indexFilename);

    long offset = index.findZipCode(zipCode);
    if (offset == -1) {
        cout << " ZIP Code " << zipCode << " not found in index.\n";
        return 0;
    }

    ifstream file(dataFilename);
    if (!file.is_open()) {
        cerr << " Error: Unable to open data file: " << dataFilename << endl;
        return 1;
    }

    file.seekg(offset);
    cout << "Seeking to file position: " << offset << endl;

    string record;
    getline(file, record);
    file.close();

    if (record.empty()) {
        cerr << " Warning: Read empty record at offset " << offset << ". Searching for nearby records...\n";
        return 1;
    }

    cout << "ZIP Code Record: " << record << endl;
    return 0;
}
