#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include "Date.h"
#include "Bst.h"
#include <string>
#include <vector>
#include <map>
#include <cmath>

// Forward declarations
class WeatherRecord;
class WeatherDataCollection;

// Custom Map wrapper for bonus marks
template<typename K, typename V>
class Map {
private:
    std::map<K, V> internalMap;

public:
    Map() = default; // Default constructor

    // Minimal but complete interface
    void insert(const K& key, const V& value) {
        internalMap[key] = value;
    }

    bool contains(const K& key) const {
        return internalMap.find(key) != internalMap.end();
    }

    V& operator[](const K& key) {
        return internalMap[key];
    }

    const V& at(const K& key) const {
        return internalMap.at(key);
    }

    // Iterator support
    typename std::map<K, V>::iterator begin() { return internalMap.begin(); }
    typename std::map<K, V>::iterator end() { return internalMap.end(); }
    typename std::map<K, V>::const_iterator begin() const { return internalMap.begin(); }
    typename std::map<K, V>::const_iterator end() const { return internalMap.end(); }
};

/// @class WeatherRecord
class WeatherRecord {
public:
    Date date;
    double windSpeed;      // S
    double temperature;    // T
    double solarRadiation; // R

    WeatherRecord(const Date& d, double ws, double temp, double sr);

    // Comparison operators for BST
    bool operator<(const WeatherRecord& other) const;
    bool operator>(const WeatherRecord& other) const;
    bool operator==(const WeatherRecord& other) const;

    friend std::ostream& operator<<(std::ostream& os, const WeatherRecord& wr);
};

/// @class WeatherDataCollection
class WeatherDataCollection {
private:
    Bst<WeatherRecord> weatherDataBST;
    Map<int, std::vector<WeatherRecord*>> dataByMonth; // Using custom map

public:
    WeatherDataCollection();
    ~WeatherDataCollection();

    // Data management
    void addWeatherRecord(const WeatherRecord& record);
    void loadFromFiles(const std::string& dataSourceFile);

    // Query operations
    std::vector<WeatherRecord> getDataForMonth(int month) const;
    std::vector<WeatherRecord> getDataForYearMonth(int year, int month) const;

    // Statistical operations
    double calculateSPCC(int month, const std::string& correlationType) const;

    // Menu option 4 calculations
    void generateMonthlyStats(int year, const std::string& filename) const;

    // Utility methods
    void displayAllData() const;
    int getTotalRecords() const;

private:
    void parseAndAddRecord(const std::string& line);
    Date parseDate(const std::string& dateTimeString);

    // Statistical helper functions
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values);
    static double calculateMAD(const std::vector<double>& values);

    // Helper method to check if month exists in map
    bool monthExists(int month) const;
};

// Statistical Functions
namespace Statistics
{
    double calculateMean(const std::vector<double>& values);
    double calculateStdDev(const std::vector<double>& values);
    double calculateMAD(const std::vector<double>& values);
    double calculateSPCC(const std::vector<double>& x, const std::vector<double>& y);
}

// Function declarations for WeatherData.cpp
void printWeatherRecord(const WeatherRecord& record);
void collectByMonth(const WeatherRecord& record, void* context);

#endif // WEATHERDATA_H
