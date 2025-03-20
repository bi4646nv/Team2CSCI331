#ifndef ZIPINDEX_H
#define ZIPINDEX_H

#include <string>
#include <map>

class ZipIndex {
private:
    std::map<std::string, long> index; // Maps Zip Code → File Offset

public:
    void buildIndex(const std::string& filename);
    void saveIndex(const std::string& indexFilename);
    void loadIndex(const std::string& indexFilename);
    long findZipCode(const std::string& zipCode) const;
};

#endif // ZIPINDEX_H
