#include "WeatherData.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

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

void WeatherDataCollection::addWeatherRecord(const WeatherRecord& record) {
    weatherDataBST.insert(record);
    int month = record.date.GetMonth();
    dataByMonth[month].push_back(const_cast<WeatherRecord*>(&record));
}

void WeatherDataCollection::loadFromFiles(const std::string& dataSourceFile) {
    std::ifstream sourceFile(dataSourceFile);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Could not open data source file: " << dataSourceFile << std::endl;
        return;
    }

    std::string filename;
    while (std::getline(sourceFile, filename)) {
        if (filename.empty()) continue;

        std::ifstream dataFile(filename);
        if (!dataFile.is_open()) {
            std::cerr << "Error: Could not open data file: " << filename << std::endl;
            continue;
        }

        std::string line;
        bool firstLine = true;

        while (std::getline(dataFile, line)) {
            if (line.empty()) continue;

            if (firstLine) {
                firstLine = false;
                continue;
            }

            parseAndAddRecord(line);
        }

        dataFile.close();
    }

    sourceFile.close();
}

void WeatherDataCollection::parseAndAddRecord(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 17) {
        std::cerr << "Error: Invalid data format in line: " << line << std::endl;
        return;
    }

    try {
        Date date = parseDate(tokens[0]);
        double windSpeed = std::stod(tokens[8]);
        double temperature = std::stod(tokens[11]);
        double solarRadiation = std::stod(tokens[12]);

        WeatherRecord record(date, windSpeed, temperature, solarRadiation);
        addWeatherRecord(record);

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

    auto it = dataByMonth.find(month);
    if (it != dataByMonth.end()) {
        for (const auto& recordPtr : it->second) {
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

// New implementation for Lab 11
std::vector<WeatherRecord> WeatherDataCollection::getDataForYearMonth(int year, int month) const
{
    std::vector<WeatherRecord> result;

    if (dataByMonth.contains(month))
    {
        for (const auto& recordPtr : dataByMonth.at(month))
        {
            if (recordPtr->date.GetYear() == year)
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

        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }

    double calculateStdDev(const std::vector<double>& values)
    {
        if (values.size() < 2)
            return 0.0;

        double mean = calculateMean(values);
        return sumSq = 0.0;

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
        return sumAbs = 0.0;

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

void WeatherDataCollection::generateMonthlyStats(int year, const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not create file " << filename << std::endl;
        return;
    }

    file << year << std::endl;

    for (int month = 1; month <= 12; month++)
    {
        auto monthlyData = getDataForYearMonth(year, month);
        if (monthlyData.empty())
            continue;

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


    }
}



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
