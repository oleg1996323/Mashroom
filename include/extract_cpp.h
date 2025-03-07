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

ErrorCode extract_cpp_pos(const std::filesystem::path& root_path,
                const std::filesystem::path& destination,
                Date from, 
                Date to,
                Coord coord,
                DATA_FORMAT format,
                DATA_OUT fmt_out);

ErrorCode extract_cpp_rect(const std::filesystem::path& root_path,
                Date from, 
                Date to,
                Rect rect,
                DATA_FORMAT format,
                DATA_OUT fmt_out){
    std::map<char,int> fmt_pos;
    std::regex reg;
    
    {
        std::string fmt;
        std::ifstream fmt_f(root_path/"format.bin",std::ios::binary|std::ios::out);
        if(!fmt_f.is_open()){
            std::cout<<root_path<<" doesn't contains \"format.bin\". Abort."<<std::endl;
            exit(1);
        }
        fmt_f.seekg(0,std::ios::end);
        unsigned end=fmt_f.tellg();
        fmt_f.seekg(0);
        fmt.resize(end);
        fmt_f.read(fmt.data(),end);
        for(int i=0;i<fmt.size();++i)
            fmt_pos[fmt[i]]=i+1;

        std::string str_reg(root_path.string());
        for(auto ch:fmt){
            switch(ch){
                case 'Y':
                str_reg = str_reg+R"(/(19[0-9]{2}|20[0-9]{2}))";
                break;
                case 'M':
                str_reg = str_reg+R"(/(0?[1-9]|1[0-2]))";
                break;
                case 'D':
                str_reg = str_reg+R"(/([1-2][0-9]|3[0-1]|0?[1-9]))";
                break;
                case 'H':
                str_reg = str_reg+R"(/(0[1-9]|[12][0-9]|3[01]))";
                break;
                case 'L':
                str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?)))";
                break;
                case 'O':
                str_reg = str_reg+R"(/(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                break;
                case 'C':
                str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?))_(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                break;
                default:
                    break;
            }
        }
        switch (format)
        {
        case DATA_FORMAT::GRIB:
            str_reg = str_reg+R"(/([^/\\]+).grib)";
            break;
        case DATA_FORMAT::BINARY:
            str_reg = str_reg+R"(/([^/\\]+).omdb)";
            break;
        default:
            break;
        }
        str_reg.append("$");
        std::cout<<str_reg<<std::endl;
        reg = str_reg;
    }
    using namespace std::chrono;
    for(const std::filesystem::directory_entry& entry:std::filesystem::recursive_directory_iterator(root_path)){
        std::smatch m;
        const std::string& p = entry.path().string();
        std::cout<<p<<std::endl;
        if(!std::regex_match(p,m,reg)){
            std::cout<<p<<" : Not matched."<<std::endl;
            continue;
        }
        else{
            
            ExtractDataCoord data;
            ValueByCoord val;
            
            if(fmt_pos.contains('Y')){
                std::ssub_match year_m = m[fmt_pos.at('Y')];
                data.date.year = std::stoi(year_m);
                if(!(from.year<=data.date.year &&
                    from.year>=data.date.year))
                    continue;
            }
            if(fmt_pos.contains('M')){
                std::ssub_match month_m = m[fmt_pos.at('M')];
                data.date.month = std::stoi(month_m);
                if(!(from.month<=data.date.month &&
                    to.month>=data.date.month))
                    continue;
            }
            if(fmt_pos.contains('D')){
                std::ssub_match day_m = m[fmt_pos.at('D')];
                data.date.day = std::stoi(day_m);
                if(!(from.day<=data.date.day &&
                    to.day>=data.date.day))
                    continue;
            }
            if(fmt_pos.contains('L')){
                std::ssub_match lat_m = m[fmt_pos.at('L')];
                // if(!(local_from_t->tm_year<=std::stoi(lat_m) &&
                //     local_to_t->tm_year>=std::stoi(lat_m)))
                //     continue;
            }
            if(fmt_pos.contains('O')){
                std::ssub_match lon_m = m[fmt_pos.at('O')];
                // if(!(local_from_t->tm_year<=std::stoi(year_m) &&
                //     local_to_t->tm_year>=std::stoi(year_m)))
                //     continue;
            }
            if(fmt_pos.contains('C'))
                std::ssub_match lon_m = m[fmt_pos.at('C')];

            //extract_val_by_coord_grib(data,entry.path().c_str(),&val,0,0);
        }
    }
}
}