#pragma once
#include <string_view>
#include <optional>
#include <code_tables/table_0.h>
#include <code_tables/table_4.h>
#include <sys/error_code.h>
#include <sys/error_print.h>
#include <sys/log_err.h>
#include <sys/application.h>
#include <./include/def.h>
#include <types/time_interval.h>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

class AbstractSearchProcess{
    protected:
    SearchProperties props_;
    fs::path in_path_;
    fs::path out_path_;
    float progress_ = 0;
    int cpus = 1;
    public:
    virtual ~AbstractSearchProcess() = default;
    virtual ErrorCode set_in_path(std::string_view in_path){
        if(!fs::exists(in_path)){
            ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,in_path.data());
            return ErrorCode::FILE_X1_DONT_EXISTS;
        }
        in_path_=in_path;
        return ErrorCode::NONE;
    }
    virtual ErrorCode set_out_path(std::string_view out_path){
        if(!fs::exists(out_path) && !fs::create_directories(out_path)){
            log().record_log(ErrorCodeLog::CREATE_DIR_X1_DENIED,"",out_path.data());
            return ErrorCode::INTERNAL_ERROR;
        }
        out_path_=out_path;
        return ErrorCode::NONE;
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
    const float& get_progress() const{
        return progress_;
    }
    void set_using_processor_cores(int cores){
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    const fs::path& out_path() const{
        return out_path_;
    }
    std::optional<Coord> get_pos() const{
        return props_.position_;
    }

    virtual ErrorCode properties_integrity() const=0;
    virtual ErrorCode execute() = 0;
};