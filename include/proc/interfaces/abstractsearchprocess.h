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

class AbstractSearchProcess{
    protected:
    SearchProperties props_; //make variant
    std::filesystem::path out_path_;
    std::unordered_set<Location<false>> in_path_;
    float progress_ = 0;
    int cpus = 1;
    public:
    AbstractSearchProcess() = default;
    AbstractSearchProcess(const AbstractSearchProcess& other) noexcept;
    AbstractSearchProcess(AbstractSearchProcess&& other) noexcept;
    AbstractSearchProcess& operator=(const AbstractSearchProcess& other) noexcept;
    AbstractSearchProcess& operator=(AbstractSearchProcess&& other) noexcept;
    virtual ~AbstractSearchProcess() = default;
    std::error_code add_in_path(std::string_view in_path);
    std::error_code add_search_host(std::string_view host,uint16_t port);
    std::error_code set_out_path(std::string_view out_path) noexcept;
    void set_center(Organization center) noexcept;
    std::optional<Organization> get_center() const noexcept;
    void set_time_fcst(TimeForecast time_fcst) noexcept;
    void add_parameter(const SearchParamTableVersion& value);
    void add_set_of_parameters(std::unordered_set<SearchParamTableVersion>&& set) noexcept;
    void add_set_of_parameters(const std::unordered_set<SearchParamTableVersion>& set);
    void add_parameter(SearchParamTableVersion&& value);
    const decltype(props_.parameters_)& get_parameters() const noexcept;
    template<typename ARG>
    void set_from_date(ARG&& from){
        props_.from_date_ = std::forward<ARG>(from);
    }
    std::optional<utc_tp_t<std::chrono::seconds>> date_from() const noexcept;
    template<typename ARG>
    void set_to_date(ARG&& to){
        props_.to_date_ = std::forward<ARG>(to);
    }
    std::optional<utc_tp_t<std::chrono::seconds>> date_to() const noexcept;
    void set_grid_respresentation(RepresentationType grid_type) noexcept;
    /** \brief Define the grid-type of the matching data.
     * 
     */
    std::optional<RepresentationType> get_grid_representation() const noexcept;
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
    const float& get_progress() const noexcept;
    /** \brief define the used parallel cores for any process
     * 
     *  \attention this field may be ignored in different case:
     *  1.  The searching path is located at hard-drive (HDD).
     *      It is defined in such a way as to exclude the damage to the hardware
     *  2.  The process is launched via Client request (this parameter can be run only
     *      directly by host - not remotely)
     */
    void set_using_processor_cores(int cores) noexcept;
    /** \brief Return the output path of process results
     * 
     * \details The result may differ: Extract (optional definition) emplace to the defined out_path_
     * the generated files requested by previously defined Properties
     * 
     */
    const std::filesystem::path& out_path() const noexcept;
    std::optional<Coord> get_pos() const noexcept;

    /** \brief Check the fullness of all necessairy fields in the search object
        \details Is pure abstract and must be overriden in the child class with
        checking its fields
        For example: Extract object needs defined Properties for correct matching of
        searched data.
    */ 
    virtual osterlib::ContextedError properties_integrity() const noexcept = 0;

    //
    virtual osterlib::ContextedError execute() noexcept = 0;
};