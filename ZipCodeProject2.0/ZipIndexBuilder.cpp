#include "ZipIndex.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

/**
 * @brief Main function for building and saving the zip code index.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument list.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << " Usage: " << argv[0] << " <datafile> <indexfile>\n";
        return 1;
    }

    string dataFilename = argv[1];
    string indexFilename = argv[2];

    ZipIndex index;
    index.buildIndex(dataFilename);
    index.saveIndex(indexFilename);

    cout << " Index created from " << dataFilename << " and saved as " << indexFilename << endl;
    return 0;
}
