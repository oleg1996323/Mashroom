#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include <memory>
#include <ranges>
#include <regex>
#include <chrono>
#include <cassert>
#include <iostream>

#ifdef __cplusplus
extern "C"{
    #include "PDSdate.h"
    #include "coords.h"
    #include "extract.h"
    #include "def.h"
}
#endif



namespace cpp{

template<bool RECTDATA,bool HOUR>
struct mapping_vals;

enum DATA_OUT{
    DEFAULT= 0,
    TXT_F = 1,
    BIN_F = 2,
    GRIB_F = 3<<1,
    ARCHIVED = 4<<2,
};

//separation by files
enum DIV_DATA_OUT{
    ALL_IN_ONE = 0, //all in one file with data inline
    YEAR_T = 1<<0,
    MONTH_T = 1<<2,
    DAY_T = 1<<3,
    HOUR_T = 1<<4,
    LAT = 1<<5,
    LON = 1<<6
};

template<>
struct mapping_vals<false,false>{
    std::map<std::string,std::vector<double>> tagged_values;
    std::pair<double,double> lat_lon_{-999,-999};
    int year_=-1;
    int month_=-1;
    int day_=-1;
};
template<>
struct mapping_vals<false,true>{
    std::map<std::string,std::vector<double>> tagged_values;
    std::pair<double,double> lat_lon_{-999,-999};
    int year_=-1;
    int month_=-1;
    int day_=-1;
    int hour_=-1;
};
template<>
struct mapping_vals<true,true>{
    std::map<std::string,std::vector<double>> tagged_values;
    GridData lat_lon_;
    int year_=-1;
    int month_=-1;
    int day_=-1;
    int hour_=-1;
};
template<>
struct mapping_vals<true,false>{
    std::map<std::string,std::vector<double>> tagged_values;
    GridData lat_lon_;
    int year_=-1;
    int month_=-1;
    int day_=-1;
};

using namespace std::chrono;

bool operator<(const Date& lhs,const Date& rhs);

bool operator<(const StampVal& lhs,const StampVal& rhs);

struct ExtractDataComparator{
    bool operator()(const Date& lhs,const Date& rhs) const{
        return lhs<rhs;
    }
    bool operator()(const StampVal& lhs,const StampVal& rhs) const{
        return lhs<rhs;
    }
};

#ifdef __cplusplus
extern "C"{
extern void extract_cpp_pos(const std::filesystem::path& root_path,
                const std::filesystem::path& destination,
                Date from, 
                Date to,
                Coord coord,
                DATA_FORMAT format, DATA_OUT out);

extern void extract_cpp_rect(const std::filesystem::path& root_path,
                Date from, 
                Date to,
                Rect rect,
                DATA_FORMAT format);
}
}
#endif