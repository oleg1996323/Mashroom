#pragma once
#include <string_view>
#include <optional>
#include "code_tables/table_0.h"
#include "code_tables/table_4.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include "sys/application.h"
#include "definitions/def.h"
#include "types/time_interval.h"
#include <filesystem>
#include <thread>
#include <netdb.h>
#include "definitions/path_process.h"
#include "types/time_interval.h"

namespace fs = std::filesystem;

class AbstractSearchProcess{
    protected:
    SearchProperties props_;
    fs::path out_path_;
    std::unordered_set<path::Storage<false>> in_path_;
    float progress_ = 0;
    int cpus = 1;
    public:
    AbstractSearchProcess() = default;
    AbstractSearchProcess(const AbstractSearchProcess& other) noexcept:
    props_(other.props_),
    out_path_(other.out_path_),
    in_path_(other.in_path_),
    progress_(other.progress_),
    cpus(other.cpus){}
    AbstractSearchProcess(AbstractSearchProcess&& other) noexcept:
    props_(std::move(other.props_)),
    out_path_(std::move(other.out_path_)),
    in_path_(std::move(other.in_path_)),
    progress_(other.progress_),
    cpus(other.cpus){}
    AbstractSearchProcess& operator=(const AbstractSearchProcess& other) noexcept{
        if(this!=&other){
            props_ = other.props_;
            out_path_ = other.out_path_;
            in_path_ = other.in_path_;
            progress_ = other.progress_;
            cpus = other.cpus;
        }
        return *this;
    }
    AbstractSearchProcess& operator=(AbstractSearchProcess&& other) noexcept{
        if(this!=&other){
            props_ = std::move(other.props_);
            out_path_ = std::move(other.out_path_);
            in_path_ = std::move(other.in_path_);
            progress_ = std::move(other.progress_);
            cpus = std::move(other.cpus);
        }
        return *this;
    }
    virtual ~AbstractSearchProcess() = default;
    ErrorCode add_in_path(std::string_view in_path){
        if(!fs::exists(in_path))
            return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,in_path.data());
        if(fs::is_regular_file(in_path))
            in_path_.insert(path::Storage<false>{in_path,path::TYPE::FILE});
        else if(fs::is_directory(in_path))
            in_path_.insert(path::Storage<false>{in_path,path::TYPE::DIRECTORY});
        else
            return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_REGULAR_FILE_OR_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,in_path);
        return ErrorCode::NONE;
    }
    ErrorCode add_search_host(std::string_view host){
        if(!gethostbyname(host.data()))
            return ErrorPrint::print_error(ErrorCode::INVALID_HOST_X1,"",AT_ERROR_ACTION::CONTINUE,host);
        in_path_.insert(path::Storage<false>{host,path::TYPE::HOST});
        return ErrorCode::NONE;
    }
    ErrorCode set_out_path(std::string_view out_path){
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
    void add_set_of_parameters(std::unordered_set<SearchParamTableVersion>&& set){
        props_.parameters_.insert(std::make_move_iterator(set.begin()),std::make_move_iterator(set.end()));
    }
    void add_set_of_parameters(const std::unordered_set<SearchParamTableVersion>& set){
        props_.parameters_.insert(set.begin(),set.end());
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
    /** \brief Define the grid-type of the matching data.
     * 
     */
    std::optional<RepresentationType> get_grid_representation() const noexcept{
        return props_.grid_type_;
    }
    /** \brief Define the global position on Earth.
     * 
     *  \remark Currently accessible only WGS coordinate-system position type.
     *  Other position coordinate-systems will be added further.
     */
    template<typename ARG>
    void set_position(ARG&& pos) noexcept{
        props_.position_.emplace(std::forward<ARG>(pos));
    }
    /** \brief Return the normilized float-point value (0-100) of current process
     *  
     *  \details Can be attached to any calling processes to notify the application.
     */
    const float& get_progress() const noexcept{
        return progress_;
    }
    /** \brief define the used parallel cores for any process
     * 
     *  \attention this field may be ignored in different case:
     *  1.  The searching path is located at hard-drive (HDD).
     *      It is defined in such a way as to exclude the damage to the hardware
     *  2.  The process is launched via Client request (this parameter can be run only
     *      directly by host - not remotely)
     */
    void set_using_processor_cores(int cores) noexcept{
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    /** \brief Return the output path of process results
     * 
     * \details The result may differ: Extract (optional definition) emplace to the defined out_path_
     * the generated files requested by previously defined Properties
     * 
     */
    const fs::path& out_path() const noexcept{
        return out_path_;
    }
    std::optional<Coord> get_pos() const noexcept{
        return props_.position_;
    }

    /** \brief Check the fullness of all necessairy fields in the search object
        \details Is pure abstract and must be overriden in the child class with
        checking its fields
        For example: Extract object needs defined Properties for correct matching of
        searched data.
    */ 
    virtual ErrorCode properties_integrity() const noexcept = 0;

    //
    virtual ErrorCode execute() noexcept = 0;
};