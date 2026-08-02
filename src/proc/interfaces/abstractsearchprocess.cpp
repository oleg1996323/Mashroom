#include "proc/interfaces/abstractsearchprocess.h"

namespace fs = std::filesystem;
    AbstractSearchProcess::AbstractSearchProcess(const AbstractSearchProcess& other) noexcept:
    props_(other.props_),
    out_path_(other.out_path_),
    in_path_(other.in_path_),
    progress_(other.progress_),
    cpus(other.cpus){}
    AbstractSearchProcess::AbstractSearchProcess(AbstractSearchProcess&& other) noexcept:
    props_(std::move(other.props_)),
    out_path_(std::move(other.out_path_)),
    in_path_(std::move(other.in_path_)),
    progress_(other.progress_),
    cpus(other.cpus){}
    AbstractSearchProcess& AbstractSearchProcess::operator=(const AbstractSearchProcess& other) noexcept{
        if(this!=&other){
            props_ = other.props_;
            out_path_ = other.out_path_;
            in_path_ = other.in_path_;
            progress_ = other.progress_;
            cpus = other.cpus;
        }
        return *this;
    }
    AbstractSearchProcess& AbstractSearchProcess::operator=(AbstractSearchProcess&& other) noexcept{
        if(this!=&other){
            props_ = std::move(other.props_);
            out_path_ = std::move(other.out_path_);
            in_path_ = std::move(other.in_path_);
            progress_ = std::move(other.progress_);
            cpus = std::move(other.cpus);
        }
        return *this;
    }
    std::error_code AbstractSearchProcess::add_in_path(std::string_view in_path){
        if(!std::filesystem::exists(in_path))
            return std::make_error_code(mashroom::errc::no_exists_path);
        if(std::filesystem::is_regular_file(in_path))
            in_path_.insert(Location<false>::file(in_path,std::chrono::system_clock::now()));
        else if(std::filesystem::is_directory(in_path))
            in_path_.insert(Location<false>::directory(in_path,std::chrono::system_clock::now()));
        else
            return std::make_error_code(mashroom::errc::not_file_or_directory);
        return {};
    }
    std::error_code AbstractSearchProcess::add_search_host(std::string_view host,uint16_t port){
        if(!gethostbyname(host.data()))
            return std::make_error_code(mashroom::errc::invalid_host);
        in_path_.insert(Location<false>::host(host,port,std::chrono::system_clock::now()));
        return {};
    }
    std::error_code AbstractSearchProcess::set_out_path(std::string_view out_path) noexcept{
        if(!std::filesystem::exists(out_path) && !std::filesystem::create_directories(out_path)){
            return std::make_error_code(mashroom::errc::create_directory_denied);
        }
        out_path_=out_path;
        return {};
    }
    void AbstractSearchProcess::set_center(Organization center) noexcept{
        props_.center_=center;
    }
    std::optional<Organization> AbstractSearchProcess::get_center() const noexcept{
        return props_.center_;
    }
    void AbstractSearchProcess::set_time_fcst(TimeForecast time_fcst) noexcept{
        props_.fcst_unit_=time_fcst;
    }
    void AbstractSearchProcess::add_parameter(const SearchParamTableVersion& value){
        props_.parameters_.insert(value);
    }
    void AbstractSearchProcess::add_set_of_parameters(std::unordered_set<SearchParamTableVersion>&& set){
        props_.parameters_.insert(std::make_move_iterator(set.begin()),std::make_move_iterator(set.end()));
    }
    void AbstractSearchProcess::add_set_of_parameters(const std::unordered_set<SearchParamTableVersion>& set){
        props_.parameters_.insert(set.begin(),set.end());
    }
    void AbstractSearchProcess::add_parameter(SearchParamTableVersion&& value){
        props_.parameters_.insert(std::move(value));
    }
    const decltype(AbstractSearchProcess::props_.parameters_)& 
        AbstractSearchProcess::get_parameters() const noexcept{
        return props_.parameters_;
    }
    std::optional<utc_tp_t<std::chrono::seconds>> 
        AbstractSearchProcess::date_from() const noexcept{
        return props_.from_date_;
    }
    std::optional<utc_tp_t<std::chrono::seconds>> AbstractSearchProcess::date_to() const noexcept{
        return props_.to_date_;
    }
    void AbstractSearchProcess::set_grid_respresentation(RepresentationType grid_type) noexcept{
        props_.grid_type_.emplace(grid_type);
    }
    /** \brief Define the grid-type of the matching data.
     * 
     */
    std::optional<RepresentationType> AbstractSearchProcess::get_grid_representation() const noexcept{
        return props_.grid_type_;
    }
    /** \brief Return the normilized float-point value (0-100) of current process
     *  
     *  \details Can be attached to any calling processes to notify the application.
     */
    const float& AbstractSearchProcess::get_progress() const noexcept{
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
    void AbstractSearchProcess::set_using_processor_cores(int cores) noexcept{
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
    const std::filesystem::path& AbstractSearchProcess::out_path() const noexcept{
        return out_path_;
    }
    std::optional<Coord> AbstractSearchProcess::get_pos() const noexcept{
        return props_.position_;
    }