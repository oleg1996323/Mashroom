#pragma once
#include <stdio.h>
#include "include/def.h"
#include "sections/section_1.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "types/data_common.h"
#include "types/data_info.h"

struct ExtractedValue{
    std::chrono::system_clock::time_point time_date;
    float value = UNDEFINED;

    bool operator<(const ExtractedValue& other) const{
        return time_date<other.time_date;
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
    Properties props_;
    fs::path root_dir_;
    fs::path dest_directory_;
    int cpus = 1;
    float progress_ = 0;
    ExtractFormat output_format_ = ExtractFormat::DEFAULT;
    ExtractedData __extract__(const fs::path& file);
    public:
    ErrorCode execute();
    void set_from_path(std::string_view root_directory){
        if(!fs::exists(root_directory))
            throw std::invalid_argument("File doesn't exists "s + root_directory.data());
        root_dir_=root_directory;
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
    template<typename ARG>
    void set_common_data(ARG&& cmn_data){
        props_.common_.emplace(std::forward<ARG>(cmn_data));
    }
    template<typename ARG>
    void set_from_date(ARG&& from){
        props_.from_date_ = std::forward<ARG>(from);
    }
    template<typename ARG>
    void set_to_date(ARG&& to){
        props_.to_date_ = std::forward<ARG>(to);
    }
    void set_time_separation(TimeSeparation t_sep){
        props_.t_sep_ = t_sep;
    }
    void set_grid_respresentation(RepresentationType grid_type){
        props_.grid_type_.emplace(grid_type);
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
};

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator|=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator&=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);