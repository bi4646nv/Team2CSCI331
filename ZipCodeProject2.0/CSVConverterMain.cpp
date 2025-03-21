/**
 * @file CSVConverterMain.cpp
 * @brief Main driver for converting CSV files to length-indicated format.
 *
 * This program reads two CSV files and converts them into length-indicated formats.
 * It uses the CSVConverter class to add length-indicated prefixes and store the output as CSV.
 */

#include <iostream>
#include "CSVConverter.h"

using namespace std;

/**
 * @brief Main function to convert CSV files to length-indicated format.
 * @return 0 on successful execution.
 */
int main() {
    // Convert both CSV files to length-indicated format
    CSVConverter::convertToLengthIndicated("us_postal_codes.csv", "us_postal_codes_length.txt");
    CSVConverter::convertToLengthIndicated("us_postal_codes_random.csv", "us_postal_codes_random_length.txt");

    cout << "CSV files successfully converted to length-indicated format." << endl;
    return 0;
}
