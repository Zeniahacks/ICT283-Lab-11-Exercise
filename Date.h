#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

class Date {
private:
    int day;
    int month;
    int year;

public:
    // Constructors
    Date();
    Date(int d, int m, int y);

    // Getters
    int GetDay() const;
    int GetMonth() const;
    int GetYear() const;

    // Setters
    void SetDay(int d);
    void SetMonth(int m);
    void SetYear(int y);

    // String conversion
    std::string toString() const;

    // Comparison operators
    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const Date& date);
    friend std::istream& operator>>(std::istream& is, Date& date);
};

// Implementation INLINE in header
inline Date::Date() : day(1), month(1), year(2000) {}

inline Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

inline int Date::GetDay() const { return day; }

inline int Date::GetMonth() const { return month; }

inline int Date::GetYear() const { return year; }

inline void Date::SetDay(int d) { day = d; }

inline void Date::SetMonth(int m) { month = m; }

inline void Date::SetYear(int y) { year = y; }

inline std::string Date::toString() const {
    return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
}

inline bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

inline bool Date::operator>(const Date& other) const {
    return other < *this;
}

inline bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}

inline bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

inline std::ostream& operator<<(std::ostream& os, const Date& date) {
    os << date.day << "/" << date.month << "/" << date.year;
    return os;
}

inline std::istream& operator>>(std::istream& is, Date& date) {
    char slash1, slash2;
    is >> date.day >> slash1 >> date.month >> slash2 >> date.year;
    return is;
}

#endif // DATE_H
