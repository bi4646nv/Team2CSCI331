/**
 * @file ZipCodeRecord.h
 * @brief Definition of the ZipCodeRecord class representing a Zip Code entry
 */

#ifndef ZIPCODE_RECORD_H
#define ZIPCODE_RECORD_H

#include <string>
#include <sstream>
#include <vector>

/**
 * @class ZipCodeRecord
 * @brief Class representing a single Zip Code record
 */
class ZipCodeRecord {
private:
    std::string zipCode;      ///< The Zip Code (primary key)
    std::string cityName;     ///< City name
    std::string stateName;    ///< State name
    std::string countyName;   ///< County name
    double latitude;          ///< Latitude coordinate
    double longitude;         ///< Longitude coordinate

public:
    /**
     * @brief Default constructor
     */
    ZipCodeRecord() : latitude(0.0), longitude(0.0) {}

    /**
     * @brief Parameterized constructor
     * @param zip The Zip Code
     * @param city The city name
     * @param state The state name
     * @param county The county name
     * @param lat The latitude
     * @param lon The longitude
     */
    ZipCodeRecord(const std::string& zip, const std::string& city, const std::string& state, 
                  const std::string& county, double lat, double lon) 
        : zipCode(zip), cityName(city), stateName(state), countyName(county), 
          latitude(lat), longitude(lon) {}

    /**
     * @brief Create a record from a comma-separated string
     * @param csvLine A comma-separated string representing the record
     * @return A ZipCodeRecord object
     */
    static ZipCodeRecord fromCSV(const std::string& csvLine) {
        std::vector<std::string> parts;
        std::stringstream ss(csvLine);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            parts.push_back(item);
        }
        
        if (parts.size() >= 6) {
            double lat = 0.0, lon = 0.0;
            try {
                lat = std::stod(parts[4]);
                lon = std::stod(parts[5]);
            } catch (...) {
                // Handle conversion errors
            }
            return ZipCodeRecord(parts[0], parts[1], parts[2], parts[3], lat, lon);
        }
        
        return ZipCodeRecord(); // Return an empty record if parsing fails
    }

    /**
     * @brief Convert the record to a comma-separated string
     * @return A comma-separated string representing the record
     */
    std::string toCSV() const {
        std::stringstream ss;
        ss << zipCode << "," << cityName << "," << stateName << "," 
           << countyName << "," << latitude << "," << longitude;
        return ss.str();
    }

    /**
     * @brief Get the Zip Code (primary key)
     * @return The Zip Code
     */
    std::string getZipCode() const { return zipCode; }

    /**
     * @brief Get the city name
     * @return The city name
     */
    std::string getCityName() const { return cityName; }

    /**
     * @brief Get the state name
     * @return The state name
     */
    std::string getStateName() const { return stateName; }

    /**
     * @brief Get the county name
     * @return The county name
     */
    std::string getCountyName() const { return countyName; }

    /**
     * @brief Get the latitude
     * @return The latitude
     */
    double getLatitude() const { return latitude; }

    /**
     * @brief Get the longitude
     * @return The longitude
     */
    double getLongitude() const { return longitude; }

    /**
     * @brief Set the Zip Code
     * @param zip The Zip Code
     */
    void setZipCode(const std::string& zip) { zipCode = zip; }

    /**
     * @brief Set the city name
     * @param city The city name
     */
    void setCityName(const std::string& city) { cityName = city; }

    /**
     * @brief Set the state name
     * @param state The state name
     */
    void setStateName(const std::string& state) { stateName = state; }

    /**
     * @brief Set the county name
     * @param county The county name
     */
    void setCountyName(const std::string& county) { countyName = county; }

    /**
     * @brief Set the latitude
     * @param lat The latitude
     */
    void setLatitude(double lat) { latitude = lat; }

    /**
     * @brief Set the longitude
     * @param lon The longitude
     */
    void setLongitude(double lon) { longitude = lon; }

    /**
     * @brief Comparison operator for sorting by Zip Code
     * @param other Another ZipCodeRecord
     * @return true if this record's Zip Code is less than the other's
     */
    bool operator<(const ZipCodeRecord& other) const {
        return zipCode < other.zipCode;
    }

    /**
     * @brief Equality operator
     * @param other Another ZipCodeRecord
     * @return true if the Zip Codes are the same
     */
    bool operator==(const ZipCodeRecord& other) const {
        return zipCode == other.zipCode;
    }
};

#endif // ZIPCODE_RECORD_H
