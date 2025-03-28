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
#include "sys/error_code.h"

#ifdef __cplusplus
extern "C"{
    #include "sections/section_1.h"
    #include "types/coord.h"
    #include "extract.h"
    #include "def.h"
}
#endif

namespace cpp{

enum DATA_OUT:uint{
    UNDEF = 1,
    DEFAULT= 0,
    TXT_F = (1<<2),
    BIN_F = (1<<3),
    GRIB_F = (1<<4),
    ARCHIVED = (1<<5)
};

DATA_OUT operator|(DATA_OUT lhs,DATA_OUT rhs);
DATA_OUT operator&(DATA_OUT lhs,DATA_OUT rhs);
DATA_OUT& operator|=(DATA_OUT& lhs,DATA_OUT rhs);
DATA_OUT& operator&=(DATA_OUT& lhs,DATA_OUT rhs);
DATA_OUT operator^(DATA_OUT lhs,DATA_OUT rhs);

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

template<bool RECTDATA,bool HOUR>
struct mapping_vals;

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

bool operator<(const StampVal& lhs,const StampVal& rhs);

struct ExtractDataComparator{
    bool operator()(const StampVal& lhs,const StampVal& rhs) const{
        return lhs<rhs;
    }
};

ErrorCode extract_cpp_pos(const std::filesystem::path& root_path,
                const std::filesystem::path& destination,
                Date from, 
                Date to,
                Coord coord,
                DATA_OUT fmt_out,
                float* progress=nullptr);

ErrorCode extract_cpp_rect(const std::filesystem::path& root_path,
                Date from, 
                Date to,
                Rect rect,
                DATA_OUT fmt_out,
                float* progress=nullptr);
}