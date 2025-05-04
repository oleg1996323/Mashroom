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

#include <abstractsearchprocess.h>
#include <abstracttimeseparation.h>
#include <abstractcallholder.h>
class Extract:public AbstractCallHolder,public AbstractSearchProcess,public AbstractTimeSeparation{
    public:
    enum class ExtractFormat:uint{
        DEFAULT = 0,
        TXT_F = 1,
        BIN_F = (1<<1),
        GRIB_F = (1<<2),
        ARCHIVED = (1<<3)
    };
    
    private:
    std::string directory_format = std::string("{:%Y")+fs::path::preferred_separator+"%m}";
    std::string file_format = std::string("{}_{}_{:%Y_%m}");
    TimeOffset diff_time_interval_ = {years(0),months(1),days(0),hours(0),minutes(0),std::chrono::seconds(0)};
    ExtractFormat output_format_ = ExtractFormat::DEFAULT;
    void __extract__(const fs::path& file,ExtractedData& ref_data,const SublimedDataInfo& positions);
    ErrorCode __create_dir_for_file__(const fs::path& out_f_name);
    ErrorCode __create_file_and_write_header__(std::ofstream& file,const fs::path& out_f_name,const ExtractedData& result);
    template<typename... ARGS>
    fs::path __generate_name__(Extract::ExtractFormat format,ARGS&&... args);
    template<typename... ARGS>
    fs::path __generate_directory__(ARGS&&... args);//TODO expand extract division
    public:
    Extract(Caller caller = hProgram.get()):AbstractCallHolder(std::move(caller)){}
    virtual ErrorCode execute() override final;
    virtual ErrorCode properties_integrity() const override final{
        if(!in_path_.empty() && !fs::exists(in_path_))
            return ErrorPrint::print_error(ErrorCode::INVALID_PATH_OF_DIRECTORIES_X1,"",AT_ERROR_ACTION::CONTINUE,in_path_.c_str());
        if(out_path_.empty())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Output path for extraction mode",AT_ERROR_ACTION::CONTINUE);
        if(!fs::exists(out_path_)){
            if(out_path_.has_extension())
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,out_path_.c_str());
            else if(!fs::create_directories(out_path_))
                return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,out_path_.c_str());
        }
        else{
            if(!fs::is_directory(out_path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,out_path_.c_str());
        }
        if(!is_correct_interval(props_.from_date_,props_.to_date_))
            return ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::CONTINUE);
        else if(!props_.position_.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Not defined",AT_ERROR_ACTION::CONTINUE);
        else if(!is_correct_pos(props_.position_.value()))
            return ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE);
        else if(!props_.grid_type_.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Grid type",AT_ERROR_ACTION::CONTINUE);
        else if(props_.parameters_.empty())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Parameters",AT_ERROR_ACTION::CONTINUE);
        if(!props_.position_.has_value() || !props_.position_.value().is_correct_pos()) //actually for WGS84
            return ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }
    void set_output_format(ExtractFormat format){
        output_format_ = format;
    }
    ExtractFormat output_format() const{
        return output_format_;
    }
    virtual void __set_offset_time_interval__() override final{
        std::string file_format_tmp;
        std::string dir_format_tmp;
        if(diff_time_interval_.hours_>std::chrono::seconds(0)){
            file_format_tmp+="S%_M%_H%_";
            dir_format_tmp+="S%"+fs::path::preferred_separator;
            dir_format_tmp+="M%"+fs::path::preferred_separator;
            dir_format_tmp+="H%"+fs::path::preferred_separator;
        }
        else if(diff_time_interval_.hours_>minutes(0)){
            file_format_tmp+="M%_H%_";
            dir_format_tmp+="M%"+fs::path::preferred_separator;
            dir_format_tmp+="H%"+fs::path::preferred_separator;
        }
        else if(diff_time_interval_.hours_>hours(0)){
            file_format_tmp+="H%_";
            dir_format_tmp+="H%"+fs::path::preferred_separator;
        }
        if(diff_time_interval_.days_>days(0)){
            file_format_tmp+="d%_m%_";
            dir_format_tmp+="d%"+fs::path::preferred_separator;
            dir_format_tmp+="m%"+fs::path::preferred_separator;
        }
        else if(diff_time_interval_.months_>months(0)){
            file_format_tmp+="m%_";
            dir_format_tmp+="m%"+fs::path::preferred_separator;
        }
        file_format_tmp+="Y%";
        dir_format_tmp+="Y%";
        std::reverse(file_format_tmp.begin(),file_format_tmp.end());
        std::reverse(dir_format_tmp.begin(),dir_format_tmp.end());
        file_format_tmp="{}_{}_{:"+file_format_tmp+"}";
        directory_format="{:"+dir_format_tmp+"}";
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
    return out_path_/std::vformat(directory_format,std::make_format_args(args...));
}