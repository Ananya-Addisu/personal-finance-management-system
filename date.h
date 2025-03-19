#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>

class Date {
public:
    int day, month, year;  // Make these public for PaymentCompare

    Date() {
        auto now = std::chrono::system_clock::now();
        time_t now_time = std::chrono::system_clock::to_time_t(now);
        tm* now_tm = localtime(&now_time);
        
        day = now_tm->tm_mday;
        month = now_tm->tm_mon + 1;
        year = now_tm->tm_year + 1900;
    }
    
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    
    std::string toString() const {
        return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << date.toString();
        return os;
    }
    
    friend std::ofstream& operator<<(std::ofstream& ofs, const Date& date) {
        ofs << date.day << " " << date.month << " " << date.year;
        return ofs;
    }
    
    friend std::ifstream& operator>>(std::ifstream& ifs, Date& date) {
        ifs >> date.day >> date.month >> date.year;
        return ifs;
    }
}; 