#pragma once
#include <string_view>
#include <optional>
#include "grib1/code_tables.h"

#include "sys/error.h"
#include "sys/application.h"
#include "definitions/def.h"
#include "OsterLib/types/time_interval.h"
#include <filesystem>
#include <thread>
#include <netdb.h>
#include <unordered_set>
#include "Location.h"
#include "OsterLib/types/time_interval.h"

namespace fs = std::filesystem;

class AbstractSearchProcess{
    protected:
    SearchProperties props_;
    std::filesystem::path out_path_;
    std::unordered_set<Location<false>> in_path_;
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
    mashroom::errc add_in_path(std::string_view in_path){
        if(!std::filesystem::exists(in_path))
            return mashroom::no_exists_path;
        if(std::filesystem::is_regular_file(in_path))
            in_path_.insert(Location<false>::file(in_path,std::chrono::system_clock::now()));
        else if(std::filesystem::is_directory(in_path))
            in_path_.insert(Location<false>::directory(in_path,std::chrono::system_clock::now()));
        else
            return mashroom::not_file_or_directory;
        return mashroom::errc::NONE;
    }
    mashroom::errc add_search_host(std::string_view host,uint16_t port){
        if(!gethostbyname(host.data()))
            return ErrorPrint::print_error(mashroom::errc::INVALID_HOST_X1,"",AT_ERROR_ACTION::CONTINUE,host);
        in_path_.insert(Location<false>::host(host,port,std::chrono::system_clock::now()));
        return mashroom::errc::NONE;
    }
    mashroom::errc set_out_path(std::string_view out_path){
        if(!std::filesystem::exists(out_path) && !std::filesystem::create_directories(out_path)){
            return mashroom::errc::create_directory_denied;
        }
        out_path_=out_path;
        return mashroom::errc::NONE;
    }
    void set_center(Organization center){
        props_.center_=center;
    }
    std::optional<Organization> get_center() const{
        return props_.center_;
    }
    void set_time_fcst(TimeForecast time_fcst){
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
    std::optional<utc_tp_t<std::chrono::seconds>> date_from() const{
        return props_.from_date_;
    }
    template<typename ARG>
    void set_to_date(ARG&& to){
        props_.to_date_ = std::forward<ARG>(to);
    }
    std::optional<utc_tp_t<std::chrono::seconds>> date_to() const{
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
    const std::filesystem::path& out_path() const noexcept{
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
    virtual mashroom::errc properties_integrity() const noexcept = 0;

    //
    virtual mashroom::errc execute() noexcept = 0;
};