#pragma once
#include <stdio.h>
#include "include/def.h"
#include "sections/section_1.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "data/common.h"
#include "data/info.h"

struct ExtractedValue{
    std::chrono::system_clock::time_point time_date;
    float value = UNDEFINED;

    ExtractedValue() = default;
    ExtractedValue(utc_tp time, float val):time_date(time),value(val){}
    ExtractedValue(const ExtractedValue& other){
        if(this!=&other){
            time_date = other.time_date;
            value = other.value;
        }
    }
    ExtractedValue(ExtractedValue&& other) noexcept{
        if(this!=&other){
            time_date = other.time_date;
            value = other.value;
        }
    }

    bool operator<(const ExtractedValue& other) const{
        return time_date<other.time_date;
    }

    bool operator>(const ExtractedValue& other) const{
        return time_date>other.time_date;
    }

    ExtractedValue& operator=(const ExtractedValue& extracted_val){
        if(this!=&extracted_val){
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
    ExtractedValue& operator=(ExtractedValue&& extracted_val) noexcept{
        if(this!=&extracted_val){
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
};

template<>
struct std::less<ExtractedValue>{
    bool operator()(const ExtractedValue& lhs,const ExtractedValue& rhs) const{
        return lhs<rhs;
    }
    bool operator()(const ExtractedValue& lhs,const ExtractedValue& rhs){
        return lhs<rhs;
    }
};

template<>
class std::hash<ExtractedValue>{
    size_t operator()(const ExtractedValue& data){
        return std::hash<int64_t>{}(static_cast<int64_t>(duration_cast<hours>(data.time_date.time_since_epoch()).count())<<32|static_cast<int64_t>(data.value));
    }
};

using ExtractedData = std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>>;
using namespace std::string_literals;
namespace fs = std::filesystem;

class Extract{
    public:
    enum class ExtractFormat:uint{
        DEFAULT = 0,
        TXT_F = 1,
        BIN_F = (1<<1),
        GRIB_F = (1<<2),
        ARCHIVED = (1<<3)
    };
    
    private:
    SearchProperties props_;
    fs::path file_;
    fs::path dest_directory_;
    std::string directory_format = std::string("{:%Y")+fs::path::preferred_separator+"%m}";
    std::string file_format = std::string("{}_{}_{:%Y_%m}");
    TimeOffset diff_time_interval_ = {years(0),months(1),days(0),hours(0),minutes(0),std::chrono::seconds(0)};
    int cpus = 1;
    float progress_ = 0;
    ExtractFormat output_format_ = ExtractFormat::DEFAULT;
    void __extract__(const fs::path& file,ExtractedData& ref_data,const SublimedDataInfo& positions);
    ErrorCode __create_dir_for_file__(const fs::path& out_f_name);
    ErrorCode __create_file_and_write_header__(std::ofstream& file,const fs::path& out_f_name,const ExtractedData& result);
    template<typename... ARGS>
    fs::path __generate_name__(Extract::ExtractFormat format,ARGS&&... args);
    template<typename... ARGS>
    fs::path __generate_directory__(ARGS&&... args);//TODO expand extract division
    utc_tp __get_next_period__(utc_tp current_time) {
        using namespace std::chrono;
        auto ymd = year_month_day(floor<days>(current_time));
        ymd+=diff_time_interval_.years_;
        ymd+=diff_time_interval_.months_;
        auto sd = utc_tp(sys_days(ymd));
        sd+=diff_time_interval_.days_+diff_time_interval_.hours_;
        return sd;
    }
    public:
    ErrorCode execute();
    void set_from_path(std::string_view file){
        if(!fs::exists(file))
            throw std::invalid_argument("File doesn't exists "s + file.data());
        file_=file;
    }
    ErrorCode set_dest_dir(std::string_view dest_directory){
        if(!fs::exists(dest_directory) && !fs::create_directories(dest_directory)){
            log().record_log(ErrorCodeLog::CREATE_DIR_X1_DENIED,"",dest_directory.data());
            return ErrorCode::INTERNAL_ERROR;
        }
        dest_directory_=dest_directory;
        return ErrorCode::NONE;
    }
    void set_output_format(ExtractFormat format){
        output_format_ = format;
    }
    void set_center(Organization center){
        props_.center_=center;
    }
    std::optional<Organization> get_center() const{
        return props_.center_;
    }
    void set_time_fcst(TimeFrame time_fcst){
        props_.fcst_unit_=time_fcst;
    }
    void add_parameter(const SearchParamTableVersion& value){
        props_.parameters_.insert(value);
    }
    void add_parameter(SearchParamTableVersion&& value){
        props_.parameters_.insert(std::move(value));
    }
    const decltype(props_.parameters_)& get_parameters() const{
        return props_.parameters_;
    }
    template<typename ARG>
    void set_from_date(ARG&& from){
        props_.from_date_ = std::forward<ARG>(from);
    }
    utc_tp date_from() const{
        return props_.from_date_;
    }
    template<typename ARG>
    void set_to_date(ARG&& to){
        props_.to_date_ = std::forward<ARG>(to);
    }
    utc_tp date_to() const{
        return props_.to_date_;
    }
    void set_time_separation(TimeSeparation t_sep){
        props_.t_sep_ = t_sep;
    }
    void set_grid_respresentation(RepresentationType grid_type){
        props_.grid_type_.emplace(grid_type);
    }
    std::optional<RepresentationType> get_grid_representation() const{
        return props_.grid_type_;
    }
    template<typename ARG>
    void set_position(ARG&& pos){
        props_.position_.emplace(std::forward<ARG>(pos));
    }
    ExtractFormat output_format() const{
        return output_format_;
    }
    const float& get_progress() const{
        return progress_;
    }
    void set_using_processor_cores(int cores){
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    const fs::path& from_file() const{
        return file_;
    }
    ExtractFormat extract_format() const{
        return output_format_;
    }
    std::optional<Coord> get_pos() const{
        return props_.position_;
    }
    void set_offset_time_interval(const TimeOffset& diff){
        diff_time_interval_ = diff;
        std::string file_format_tmp;
        std::string dir_format_tmp;
        if(diff.years_>years(0)){
            file_format_tmp+="%Y";
            dir_format_tmp+="%Y";
        }
        if(diff.months_>months(0)){
            if(file_format_tmp.empty())
                file_format_tmp+="%m";
            else {
                file_format_tmp+="_%m";
                dir_format_tmp+=fs::path::preferred_separator+"%m";
            }
        }
        if(diff.days_>days(0)){
            if(file_format_tmp.empty())
                file_format_tmp+="%d";
            else{
                file_format_tmp+="_%d";
                dir_format_tmp+=fs::path::preferred_separator+"%d";
            }
        }
        if(diff.hours_>hours(0)){
            if(file_format_tmp.empty())
                file_format_tmp+="%H";
            else{
                file_format_tmp+="_%H";
                dir_format_tmp+=fs::path::preferred_separator+"%H";
            }
        }
        if(!file_format_tmp.empty()){
            file_format_tmp="_{:"+file_format_tmp+"}";
            file_format ="{}_{}"+file_format_tmp;
        }
        if(!dir_format_tmp.empty())
            directory_format="{:"+dir_format_tmp+"}";
    }
    const TimeOffset& get_offset_time_interval() const{
        return diff_time_interval_;
    }
};

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator|=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator&=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator~(Extract::ExtractFormat val);

template<typename... ARGS>
fs::path Extract::__generate_name__(Extract::ExtractFormat format,ARGS&&... args){
    if( static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
        static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::TXT_F) || 
        static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
        static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::DEFAULT)){
        return std::vformat(file_format+".txt",std::make_format_args(args...));
    }
    return "any.txt";
}
template<typename... ARGS>
fs::path Extract::__generate_directory__(ARGS&&... args){
    return dest_directory_/std::vformat(directory_format,std::make_format_args(args...));
}