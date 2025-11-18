#include "WeatherData.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

// WeatherRecord implementation
WeatherRecord::WeatherRecord(const Date& d, double ws, double temp, double sr)
    : date(d), windSpeed(ws), temperature(temp), solarRadiation(sr) {}

bool WeatherRecord::operator<(const WeatherRecord& other) const {
    return date < other.date;
}

bool WeatherRecord::operator>(const WeatherRecord& other) const {
    return date > other.date;
}

bool WeatherRecord::operator==(const WeatherRecord& other) const {
    return date == other.date;
}

std::ostream& operator<<(std::ostream& os, const WeatherRecord& wr) {
    os << wr.date << " | WS: " << wr.windSpeed << " | Temp: " << wr.temperature
       << " | Solar: " << wr.solarRadiation;
    return os;
}

// WeatherDataCollection implementation
WeatherDataCollection::WeatherDataCollection()
    : weatherDataBST(), dataByMonth() {} // Initialize in member list

WeatherDataCollection::~WeatherDataCollection() {}

// Add weather record
void WeatherDataCollection::addWeatherRecord(const WeatherRecord& record)
{
    WeatherRecord* recordCopy = new WeatherRecord(record);

    weatherDataBST.insert(record);

    int month = record.date.GetMonth();

    // Use custom Map's insert method
    if (!dataByMonth.contains(month))
    {
        dataByMonth.insert(month, std::vector<WeatherRecord*>());
    }

    // Store the heap-allocated copy
    dataByMonth.at(month).push_back(recordCopy);
}

// To check if month exists
bool WeatherDataCollection::monthExists(int month) const {
    return dataByMonth.contains(month);
}

// To allow the app to load from a txt file
void WeatherDataCollection::loadFromFiles(const std::string& dataSourceFile) {
    std::ifstream sourceFile(dataSourceFile);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Could not open data source file: " << dataSourceFile << std::endl;
        return;
    }

    std::string filename;
    int fileProcessed = 0;

    std::cout << "Reading files from: " << dataSourceFile << std::endl;

    while (std::getline(sourceFile, filename)) {
        if (filename.empty() || filename[0] == '#')
            continue;

        // Remove any extra whitespace
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);

        if (filename.empty()) continue;

        std::cout << "Processing file: " << filename << std::endl;

        std::ifstream dataFile(filename);
        if (!dataFile.is_open())
        {
            std::cerr << "Error: Could not open data file: " << filename << std::endl;
            continue;
        }

        std::string line;
        bool firstLine = true;
        int recordsProcessed = 0;

        while (std::getline(dataFile, line)) {
            if (line.empty()) continue;

            if (firstLine)
            {
                if (line.find("WAST") != std::string::npos || line.find("Date") != std::string::npos)
                {
                    firstLine = false;
                }
                continue;
            }
            parseAndAddRecord(line);
            recordsProcessed++;
        }

        dataFile.close();

        fileProcessed++;
    }

    sourceFile.close();
}

// Parse and add record
void WeatherDataCollection::parseAndAddRecord(const std::string& line) {
    // Skip lines that are just commas or whitespace
    if (line.find_first_not_of(" ,\t\r\n") == std::string::npos) {
        return;
    }

    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    try {
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Check if we have enough columns
        if (tokens.size() < 18) {
            std::cerr << "Warning: Skipping line with insufficient columns: " << line << std::endl;
            return;
        }

        // Parse date
        Date date = parseDate(tokens[0]);

        // Parse numerical values - CORRECT COLUMN INDICES:
        // Based on your CSV: WAST,DP,Dta,Dts,EV,QFE,QFF,QNH,RF,RH,S,SR,T,ST1,ST2,ST3,ST4,Sx
        // Columns: 0=Date, 8=S (wind speed), 11=SR (solar radiation), 12=T (temperature)
        double windSpeed = std::stod(tokens[10]);      // Column 11: S (wind speed)
        double solarRadiation = std::stod(tokens[11]); // Column 12: SR (solar radiation)
        double temperature = std::stod(tokens[17]);    // Column 18: T (temperature)

        WeatherRecord record(date, windSpeed, temperature, solarRadiation);
        addWeatherRecord(record);

        // Debug
        std::cout << "Parsed record: " << date << " WS: " << windSpeed << " Temp: " << temperature << " Solar: " << solarRadiation << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
    }
}

Date WeatherDataCollection::parseDate(const std::string& dateTimeString) {
    size_t spacePos = dateTimeString.find(' ');
    if (spacePos == std::string::npos) {
        throw std::invalid_argument("Invalid date format");
    }

    std::string datePart = dateTimeString.substr(0, spacePos);
    std::stringstream ss(datePart);

    int day, month, year;
    char slash1, slash2;

    if (!(ss >> day >> slash1 >> month >> slash2 >> year)) {
        throw std::invalid_argument("Failed to parse date");
    }

    // Validate date components
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        throw std::invalid_argument("Invalid date values: " + datePart);
    }

    return Date(day, month, year);
}

std::vector<WeatherRecord> WeatherDataCollection::getDataForMonth(int month) const {
    std::vector<WeatherRecord> result;

    if (monthExists(month))
        {
        // Use at() to access the vector
        const auto& records = dataByMonth.at(month);
        for (const auto& recordPtr : records)
        {
            result.push_back(*recordPtr);
        }
    }

    return result;
}

// Function pointer implementations
void printWeatherRecord(const WeatherRecord& record) {
    std::cout << record << std::endl;
}

struct CollectionContext {
    std::vector<WeatherRecord>* records;
    int targetMonth;
};

void collectByMonth(const WeatherRecord& record, void* context) {
    CollectionContext* ctx = static_cast<CollectionContext*>(context);
    if (record.date.GetMonth() == ctx->targetMonth) {
        ctx->records->push_back(record);
    }
}

// Calculation of SPCC
double WeatherDataCollection::calculateSPCC(int month, const std::string& correlationType) const {
    auto monthlyData = getDataForMonth(month);
    if (monthlyData.empty()) {
        return 0.0;
    }

    std::vector<double> x, y;

    for (const auto& record : monthlyData) {
        if (correlationType == "S_T") {
            x.push_back(record.windSpeed);
            y.push_back(record.temperature);
        } else if (correlationType == "S_R") {
            x.push_back(record.windSpeed);
            y.push_back(record.solarRadiation);
        } else if (correlationType == "T_R") {
            x.push_back(record.temperature);
            y.push_back(record.solarRadiation);
        }
    }

    return Statistics::calculateSPCC(x, y);
}

// New implementation for Lab 11 / get the data for the month and year
std::vector<WeatherRecord> WeatherDataCollection::getDataForYearMonth(int year, int month) const
{
    std::vector<WeatherRecord> result;

    if (monthExists(month))
    {
        for (const auto& recordPtr : dataByMonth.at(month))
        {
            if (recordPtr && recordPtr->date.GetYear() == year)
            {
                result.push_back(*recordPtr);
            }
        }
    }

    return result;
}

// Statistical namespace implementation
namespace Statistics
{
    double calculateMean(const std::vector<double>& values)
    {
        if (values.empty())
            return 0.0;

        double sum = 0.0;
        for (double val : values)
        {
            sum += val;
        }
        return sum / values.size();
    }

    double calculateStdDev(const std::vector<double>& values)
    {
        if (values.size() < 2)
            return 0.0;

        double mean = calculateMean(values);
        double sumSq = 0.0;

        for (double val : values)
        {
            sumSq += (val - mean) * (val - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }

    double calculateMAD(const std::vector<double>& values)
    {
        if (values.empty())
            return 0.0;

        double mean = calculateMean(values);
        double sumAbs = 0.0;

        for (double val : values)
        {
            sumAbs += std::abs(val - mean);
        }
        return sumAbs / values.size();
    }

    double calculateSPCC(const std::vector<double>& x, const std::vector<double>& y)
    {
        if (x.size() != y.size() || x.size() < 2)
            return 0.0;

        int n = x.size();
        double sum_x = 0.0, sum_y = 0.0;
        double sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

        for (int i = 0; i < n; ++i) {
           sum_x += x[i];
           sum_y += y[i];
           sum_xy += x[i] * y[i];
           sum_x2 += x[i] * x[i];
           sum_y2 += y[i] * y[i];
    }

    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = std::sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));

    // Use epsilon for floating point comparison instead of ==
    if (std::abs(denominator) < 1e-10)
        return 0.0;

    return numerator / denominator;

    }
}

// Generate Monthly statistics for the weather report
void WeatherDataCollection::generateMonthlyStats(int year, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not create file " << filename << std::endl;
        return;
    }

    // Write the year header exactly as specified
    file << year << std::endl;

    std::string monthNames[] = {"January", "February", "March", "April", "May", "June",
                               "July", "August", "September", "October", "November", "December"};

    for (int month = 1; month <= 12; month++)
    {
        auto monthlyData = getDataForYearMonth(year, month);

        if (monthlyData.empty())
        {
            // Write empty data for months with no data
            file << monthNames[month-1] << ",0.0(0.0, 0.0),0.0(0.0, 0.0),0.0" << std::endl;
            continue;
        }

        // Extract data for calculations
        std::vector<double> windSpeeds, temperatures, solarRads;
        double totalSolar = 0.0;

        for (const auto& record : monthlyData)
        {
            windSpeeds.push_back(record.windSpeed);
            temperatures.push_back(record.temperature);
            solarRads.push_back(record.solarRadiation);
            totalSolar += record.solarRadiation;
        }

        // Calculate statistics using DECOUPLED functions
        double avgWind = Statistics::calculateMean(windSpeeds);
        double avgTemp = Statistics::calculateMean(temperatures);
        double stdWind = Statistics::calculateStdDev(windSpeeds);
        double stdTemp = Statistics::calculateStdDev(temperatures);
        double madWind = Statistics::calculateMAD(windSpeeds);
        double madTemp = Statistics::calculateMAD(temperatures);

        // Write in EXACT format: Month,AvgWS(std,mad),AvgTemp(std,mad),TotalSolar
        file << monthNames[month-1] << ","
             << avgWind << "(" << stdWind << ", " << madWind << "),"
             << avgTemp << "(" << stdTemp << ", " << madTemp << "),"
             << totalSolar << std::endl;
    }

    file.close();
    std::cout << "Monthly statistics written to " << filename << std::endl;
}

// Display all data
void WeatherDataCollection::displayAllData() const {
    std::cout << "=== All Weather Data (" << getTotalRecords() << " records) ===" << std::endl;
    weatherDataBST.inOrder([](const WeatherRecord& record)
    {
        std::cout << record << std::endl;
    });
}

int WeatherDataCollection::getTotalRecords() const {
    return weatherDataBST.size();
}
