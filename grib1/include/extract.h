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


namespace fs = std::filesystem;
std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>> extract(const std::chrono::system_clock::time_point& dfrom, const std::chrono::system_clock::time_point& dto,const Coord& coord,const fs::path& ffrom,
                                                                    std::optional<TimeFrame> fcst_unit={},std::optional<Organization> center={}, std::optional<uint8_t> subcenter={});

class Extract{
    public:
    enum class ExtractFormat:uint{
        TXT_F = 0,
        BIN_F = (1),
        GRIB_F = (1<<2),
        ARCHIVED = (1<<3)
    };
    struct Properties{
        std::optional<CommonDataProperties> common_;
        std::optional<std::chrono::system_clock::time_point> from_date_;
        std::optional<std::chrono::system_clock::time_point> to_date_;
        std::optional<TimeSeparation> t_sep_;
        std::optional<RepresentationType> grid_type_;
        std::optional<Coord> position_;
    };
    
    private:
    GribDataInfo result;
    std::string_view root_dir_;
    std::string_view dest_directory_;
    int cpus = 1;
    ExtractFormat output_format_ = ExtractFormat::TXT_F;
    public:
    const GribDataInfo& execute();
    void set_from_path(std::string_view root_directory){
        if(!fs::exists(root_directory))
            throw std::invalid_argument("File doesn't exists "s + root_directory.data());
        root_dir_=root_directory;
    }
    void set_dest_dir(std::string_view dest_directory){
        if(!fs::exists(dest_directory) && !fs::create_directories(dest_directory))
            throw std::invalid_argument("Unable to create capitalize destination path "s + dest_directory.data());
        dest_directory_=dest_directory;
    }
    void set_output_type(ExtractFormat format){
        output_format_ = format;
    }
    const GribDataInfo& get_result() const{
        return result;
    }
    ExtractFormat output_format() const{
        return output_format_;
    }
    void clear_result(){
        result.info_.clear();
        result.err = ErrorCodeData::NONE_ERR;
    }
    void set_using_processor_cores(int cores){
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    GribDataInfo&& release_result() noexcept{
        GribDataInfo res(std::move(result));
        return res;
    }
};

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator|=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat& operator&=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs);
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs);