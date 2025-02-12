#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <map>
#include <limits>


// Storing the zip code data
struct ZipData {
    int zipCode;
    std::string placeName;
    std::string state;
    std::string county;
    double lat;
    double lon;
};

//function to compare and get the (min/max)for each state
void processData(cons std::string& inputFileName,
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);

    // checking input file
    if (!inputFile.is_open()) {
        std::cerr << "Input file couldn't open!" << std::endl;
        return;
    }

    // checking output file
    if (!outputFile.is_open()) {
        std::cerr << "Output file couldn't open"
    }

// Skip the header
std::string line;
std::getline(inputFile, line);




int main() {
    std::ifstream file("us_postal_codes.csv");
    std::string line;

    // Read file line by line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string column;

        // Print each column in the row 

        while (std::getline(ss, column, ',')) {
            std::cout << column << " | ";
        }
        std::cout << std::endl;
    }

    file.close();
    return 0;
}
