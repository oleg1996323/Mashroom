#pragma once
#include <decode.h>
//#include <grib1/include/capitalize.h>
// #include <extract.h>
// #include <interpolation.h>
// #include <structures.h>
// #include <contains.h>
#include <filesystem>
#include <string>

struct OrderItems{
    int hour = -1;
    int day = -1;
    int month = -1;
    int year = -1;
    int lat = -1;
    int lon = -1;
    DataFormat fmt = DataFormat::NONE;
};

// #define OrderItems(...) ((OrderItems){.hour=-1,.day=-1,.month=-1,.year=-1,.lat=-1,.lon=-1,.fmt = NONE},##__VA_ARGS__)

void capitalize_cpp(const std::filesystem::path& root,const std::filesystem::path& from,const OrderItems& order);