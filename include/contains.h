#pragma once
#include "def.h"
#include "types/coord.h"
#include "sections/section_1.h"
#include "code_tables/table_6.h"
#include "types/time_interval.h"
#include "data/common.h"
#include <filesystem>
#include <data/info.h>
#include <abstractsearchprocess.h>
#include <abstracttimeseparation.h>
#include <program/mashroom.h>
namespace fs = std::filesystem;

struct FoundDataInfo{
    std::optional<GridInfo> grid_data;
    std::vector<ptrdiff_t> buf_pos_;
    std::chrono::system_clock::time_point from = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point to = std::chrono::system_clock::time_point::min();
    std::chrono::system_clock::duration discret = std::chrono::system_clock::duration(0);
    CommonDataProperties* common_props_;
    std::optional<TimeFrame> fcst_time_;
};

struct ContainOutputFilter{
    bool file = true;
    bool center = true;
    bool table_version = true;
    bool parameter = true;
    bool forecast_time = true;
    bool grid_info = true;
    bool time_interval = true;
};

class Contains:public AbstractSearchProcess{
    std::vector<FoundDataInfo> data_; //make_templated further
    std::string common_format_ = "File {}\ncenter {}({}) table version {} parameter {}({}) forecast-time {}";
    std::string grid_format_ = "{}";
    std::string time_interval_format_ = "from {:%Y/%m%d %H%M%S} to {:%Y/%m%d %H%M%S}";
    ContainOutputFilter filter_;
    void __make_formats_from_filter__();
    public:
    virtual ErrorCode execute() override final{
        if(     !props_.center_.has_value() && 
                props_.fcst_unit_.has_value() && 
                props_.from_date_==utc_tp() && 
                time_point_cast<hours>(props_.to_date_)==time_point_cast<hours>(std::chrono::system_clock::now()) && 
                !props_.grid_type_.has_value() &&
                !props_.parameters_.empty())
            for(auto& [file,file_data]:hProgram->data().data()){
                for(auto& [common,info_seq]:file_data)
                    for(auto& info:info_seq){
                        std::cout<<std::vformat(common_format_,std::make_format_args(filter_.file?file:(filter_.center?center:(filter_.table_version?))));
                        std::cout<<std::vformat(found_output_,std::make_format_args(file.data(),(int)common->center_.value(),center_to_abbr(common->center_.value()),(int)common->table_version_.value(),(int)common->parameter_.value(),
                        parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name,(int)common->fcst_unit_.value(),
                        (int)info.grid_data.value().rep_type,grid_to_abbr(info.grid_data.value().rep_type).data(),info.grid_data.value().print_grid_info(),info.from,info.to));
                    }
            }

    }
    virtual ErrorCode properties_integrity() const override final{
        
    }
};

bool contains(const fs::path& from,const utc_tp& date ,const Coord& coord,
    std::optional<RepresentationType> grid_type = {},
    std::optional<Organization> center = {},
    std::optional<uint8_t> table_version = {},
    std::optional<TimeFrame> fcst = {});

bool contains(const fs::path& from,const utc_tp& date ,const Coord& coord,
    const CommonDataProperties& data,std::optional<RepresentationType> grid_type = {});

bool contains(const fs::path& from,const utc_tp& date ,const Coord& coord,
    Organization center, uint8_t table_version, uint8_t parameter,std::optional<RepresentationType> grid_type = {});