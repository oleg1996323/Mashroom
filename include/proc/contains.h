#pragma once
#include "def.h"
#include "types/coord.h"
#include "sections/section_1.h"
#include "code_tables/table_6.h"
#include "types/time_interval.h"
#include "data/info.h"
#include <filesystem>
#include "data/sublimed_info.h"
#include "proc/interfaces/abstractsearchprocess.h"
#include "proc/interfaces/abstracttimeseparation.h"
#include "program/mashroom.h"
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
#include "proc/interfaces/abstractthreadinterruptor.h"
class Contains:public AbstractSearchProcess,public AbstractThreadInterruptor{
    std::vector<FoundDataInfo> data_; //make_templated further
    ContainOutputFilter filter_;
    bool integral_only_; //search only integral time-series in searching time interval
    public:
    virtual ErrorCode execute() override final{
        if(     !props_.center_.has_value() && 
                props_.fcst_unit_.has_value() && 
                props_.from_date_==utc_tp() && 
                time_point_cast<hours>(props_.to_date_)==time_point_cast<hours>(std::chrono::system_clock::now()) && 
                !props_.grid_type_.has_value() &&
                !props_.parameters_.empty()){
            for(auto& [file,file_data]:hProgram->data().data()){
                for(auto& [common,info_seq]:file_data)
                    for(auto& info:info_seq){
                        if(stop_token_.stop_requested())
                            return ErrorCode::INTERRUPTED;
                        if(filter_.file)
                            std::cout<<file<<std::endl;
                        if(filter_.center)
                            std::cout<<"center "<<(int)common->center_.value()<<"("<<center_to_abbr(common->center_.value())<<") "<<std::flush;
                        if(filter_.table_version)
                            std::cout<<"table version "<<(int)common->table_version_.value()<<" "<<std::flush;
                        if(filter_.parameter)
                            std::cout<<"parameter "<<(int)common->parameter_.value()<<"("<<parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name
                            <<") "<<std::flush;
                        if(filter_.forecast_time)
                            std::cout<<"forecast-time "<<(int)common->fcst_unit_.value()<<std::endl;
                        if(filter_.grid_info)
                            std::cout<<info.grid_data->print_grid_info()<<std::endl;
                        if(filter_.time_interval)
                            std::cout<<std::format("from {:%Y/%m%d %H%M%S} to {:%Y/%m%d %H%M%S}",info.from,info.to)<<std::endl;
                    }
            }
        }
        else{
            assert(false);
        }
        return ErrorCode::NONE;
    }
    void set_integral_only(bool integral);
    virtual ErrorCode properties_integrity() const override final{
        if(out_path_.empty())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"output directory",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
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