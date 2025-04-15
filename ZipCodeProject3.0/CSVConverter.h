/**
 * @file CSVConverter.h
 * @brief Header file for converting CSV files to length-indicated format.
 */

#ifndef CSVCONVERTER_H
#define CSVCONVERTER_H

#include <iostream>
#include <fstream>
#include <string>
#include "HeaderBuffer.h"

using namespace std;

/**
 * @class CSVConverter
 * @brief Handles conversion of CSV files to length-indicated format.
 */
class CSVConverter {
public:
    /**
     * @brief Converts a CSV file to a length-indicated format.
     * @param csvFilename The input CSV file.
     * @param outputFilename The output length-indicated file.
     */
    static void convertToLengthIndicated(const string& csvFilename, const string& outputFilename);
};

#endif // CSVCONVERTER_H
