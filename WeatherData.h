#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include "Date.h"
#include "Bst.h"
#include <string>
#include <vector>
#include <map>

// Forward declarations
class WeatherRecord;
class WeatherDataCollection;

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
    std::map<int, std::vector<WeatherRecord*>> dataByMonth;

public:
    WeatherDataCollection();
    ~WeatherDataCollection();

    void addWeatherRecord(const WeatherRecord& record);
    void loadFromFiles(const std::string& dataSourceFile);

    std::vector<WeatherRecord> getDataForMonth(int month) const;
    double calculateSPCC(int month, const std::string& correlationType) const;

    void displayAllData() const;
    int getTotalRecords() const;

private:
    void parseAndAddRecord(const std::string& line);
    Date parseDate(const std::string& dateTimeString);
};

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

    int size() const {
        return internalMap.size();
    }

    bool empty() const {
        return internalMap.empty();
    }

    void clear() {
        internalMap.clear();
    }

    // Iterator support
    typename std::map<K, V>::iterator begin() { return internalMap.begin(); }
    typename std::map<K, V>::iterator end() { return internalMap.end(); }
    typename std::map<K, V>::const_iterator begin() const { return internalMap.begin(); }
    typename std::map<K, V>::const_iterator end() const { return internalMap.end(); }
};

// Function declarations for WeatherData.cpp
void printWeatherRecord(const WeatherRecord& record);
void collectByMonth(const WeatherRecord& record, void* context);

#endif // WEATHERDATA_H
