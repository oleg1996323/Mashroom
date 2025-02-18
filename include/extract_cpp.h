#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include <memory>
#include <ranges>
#include <regex>
#include <chrono>
#include <cassert>

#ifdef __cplusplus
extern "C"{
    #include "PDSdate.h"
    #include "coords.h"
}
#endif

namespace cpp{

struct ReturnVal{
    Date time;
    float val;
    const char* tag;
};

ReturnVal* extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const std::filesystem::path& ffrom,const std::filesystem::path& fto,long int count, long unsigned pos);

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

template<bool RECTDATA,bool HOUR>
void extract_cpp_pos(const std::filesystem::path& root_path,
                const std::filesystem::path& destination,
                Date from, 
                Date to,
                Coord coord,
                DATA_FORMAT format){
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
    time_point<system_clock> beg = system_clock::now();
    mapping_vals<RECTDATA,HOUR> map_vals;
    {
        for(const std::filesystem::directory_entry& entry:std::filesystem::recursive_directory_iterator(root_path)){
            std::smatch m;
            const std::string& p = entry.path().string();
            std::cout<<p<<std::endl;
            if(!std::regex_match(p,m,reg)){
                std::cout<<p<<" : Not matched."<<std::endl;
                continue;
            }
            else{
                ValueByCoord val;
                if(fmt_pos.contains('Y')){
                    std::ssub_match year_m = m[fmt_pos.at('Y')];
                    int year = std::stoi(year_m);
                    if(from.year>year ||
                        to.year<year)
                        continue;
                }
                if(fmt_pos.contains('M')){
                    std::ssub_match month_m = m[fmt_pos.at('M')];
                    int month = std::stoi(month_m);
                    if(from.month>month ||
                        to.month<month)
                        continue;
                }
                if(fmt_pos.contains('D')){
                    std::ssub_match day_m = m[fmt_pos.at('D')];
                    int day = std::stoi(day_m);
                    if(from.day>day ||
                        to.day<day)
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
                
                // std::ofstream output(root_path/(m[m.size()-1].str()+".txt"),std::ios::ate);
                // if(!output.is_open()){
                //     std::cout<<root_path/(m[m.size()-1].str()+".txt")<<" not openned. Abort."<<std::endl;
                // }
                
                auto result = extract_val_by_coord_grib(from,to,coord,entry.path().c_str(),(destination/(m[m.size()-1].str()+".txt")).c_str(),&val,0,0);
                //output<<result.time.day<<"/"<<result.time.month<<"/"<<result.time.year<<" "<<result.time.hour<<":"<<result.val<<std::endl;
            }
        }
        std::cout<<"Duration: "<<duration_cast<seconds>(system_clock::now()-beg)<<std::endl;
    }
}

template<bool RECTDATA,bool HOUR>
void extract_cpp_rect(const std::filesystem::path& root_path,
                Date from, 
                Date to,
                Rect rect,
                DATA_FORMAT format){
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
    mapping_vals<RECTDATA,HOUR> map_vals;
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

/*
void extract_cpp(const std::filesystem::path& root_path,
                std::chrono::time_point<std::chrono::system_clock> from, 
                std::chrono::time_point<std::chrono::system_clock> to){
    std::string fmt;
    {
        std::ifstream fmt_f("format.bin",std::ios::binary);
        fmt_f.seekg(0,std::ios::end);
        unsigned end=fmt_f.tellg();
        fmt_f.seekg(0);
        fmt.resize(end);
        fmt_f.read(fmt.data(),end);
    }
    std::filesystem::path str_reg("^"+root_path.string());
    for(char ch:fmt){
        switch(ch){
            case 'Y':
            str_reg = str_reg/"(19[0-9]{2}|20[0-9]{2})";
            break;
            case 'M':
            str_reg = str_reg/"(0[1-9]|1[0-2])";
            break;
            case 'D':
            str_reg = str_reg/"[1-2][0-9]|3[0-1]|0?[1-9]";
            break;
            case 'H':
            str_reg = str_reg/"(0[1-9]|[12][0-9]|3[01])";
            break;
            case 'L':
            str_reg = str_reg/"(lat-?(90(.[0]+)?|[0-8]?[0-9](.[0-9]+)?))";
            break;
            case 'O':
            str_reg = str_reg/"(lon-?(180(.[0]+)?|1[0-7]?[0-9](.[0-9]+)?|0?[0-9]?[0-9](.[0-9]+)?))";
            break;
            case 'C':
            str_reg = str_reg/"(lat-?(90(.[0]+)?|[0-8]?[0-9](.[0-9]+)?))_(lon-?(180(.[0]+)?|1[0-7]?[0-9](.[0-9]+)?|0?[0-9]?[0-9](.[0-9]+)?))";
            break;
            default:
        }
    }
    str_reg.append("$");
    using namespace std::chrono;
    time_t from_t = system_clock::to_time_t(from);
    time_t to_t = system_clock::to_time_t(to);
    tm* local_from_t = localtime(&from_t);
    tm* local_to_t = localtime(&to_t);
    DirList root;
    root.cur_dir = std::filesystem::directory_iterator(root_path);
    DirList* cur_dir = &root;
    size_t count = 0;
    for(const std::filesystem::directory_entry& entry:std::filesystem::recursive_directory_iterator(root_path)){
        if(count<fmt.size()){
            switch(fmt.at(count)){
                case 'Y':
                break;
                case 'M':
                break;
                case 'D':
                break;
                case 'H':
                break;
                case 'L':
                break;
                case 'O':
                break;
                case 'C':
                break;
                default:
            }
            cur_dir->next = std::make_unique<DirList>();
            cur_dir->next->ex = cur_dir;
            cur_dir->next->cur_dir = std::filesystem::directory_iterator(cur_dir->get().path());
            cur_dir = cur_dir->next.get();
            ++count;
        }
        else{
            --count;
            if(cur_dir->ex){
                cur_dir = cur_dir->ex;
            }
            else{
                assert(count==0);
                return;
            }
        }
    }
    
    std::regex_match(cur_dir->get().path().string(),reg);
    if(std::regex_match(cur_dir->get().path().string(),reg) &&
        local_from_t->tm_year<=std::stoi(cur_dir->get().path().string()) &&
        local_to_t->tm_year>=std::stoi(cur_dir->get().path().string()))
            break;


    std::regex reg("R(^([1-9]|0[1-9]|1[0-2])$)");
                    if(std::regex_match(entry.path().string(),reg))
                        std::stoi(entry.path().string());
                    break;
}
*/
}