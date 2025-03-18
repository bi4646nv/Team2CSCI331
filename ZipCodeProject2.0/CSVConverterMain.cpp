/**
 * @file CSVConverterMain.cpp
 * @brief Main driver for converting CSV files to length-indicated format.
 */

#include <iostream>
#include "CSVConverter.h"

using namespace std;

int main() {
    // Convert both CSV files to length-indicated format
    CSVConverter::convertToLengthIndicated("us_postal_codes.csv", "us_postal_codes_length.txt");
    CSVConverter::convertToLengthIndicated("us_postal_codes_random.csv", "us_postal_codes_random_length.txt");

    cout << "CSV files successfully converted to length-indicated format." << endl;
    return 0;
}
