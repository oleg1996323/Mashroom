#pragma once
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

struct Date{
    int year = 0;
    int month = 0;
    int day = 0;
};

struct Time{
    int hour = 0;
    int minute = 0;
    int second = 0;
};

struct GridSize{
    int x;
    int y;
};

void extract_by_date(std::filesystem::path filename, const Date& date,std::filesystem::path output);

void extract_by_datetime(std::filesystem::path filename, const Date& date, const Time& time,std::filesystem::path output = std::string("./"));