#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "types/data_info.h"
#include "message.h"

using namespace std::chrono_literals;
namespace fs = std::filesystem;

template<class Clock, class Duration> 
struct std::incrementable_traits<std::chrono::time_point<Clock, Duration>> { 
  using difference_type = typename Duration::rep;
};

struct ErrorFiles{
    fs::path name;
    ErrorCodeData code;
};

struct ProcessResult{
    std::set<long long> found;
    std::vector<ErrorFiles> err_files;
};

namespace fs = std::filesystem;
using namespace std::string_literals;
static constexpr const char* miss_files_filename = "missing_files.txt";
static constexpr const char* errorness_files_filename = "corrupted_files.txt";
class Check{
    public:
    enum class TimeSeparation{
        HOUR,
        DAY,
        MONTH,
        YEAR
    };
    struct Properties{
        std::optional<CommonDataProperties> common_;
        std::optional<long long> from_date_;
        std::optional<long long> to_date_;
        std::optional<TimeSeparation> t_sep_;
        std::optional<RepresentationType> grid_type_;
        std::optional<Coord> position_;
    };
    private:
    Properties props_;
    GribDataInfo result;
    fs::path root_directory_; //input root directory
    fs::path dest_directory_; //output log directory
    int cpus = 1;
    bool missing_files();
    public:
    static bool check_format(std::string_view fmt);
    const GribDataInfo& execute();
    void set_checking_directory(std::string_view dir){
        if(!fs::exists(dir))
            ErrorPrint::print_error(ErrorCode::DIRECTORY_X1_DONT_EXISTS,"",AT_ERROR_ACTION::ABORT,dir.data());
        if(!fs::is_directory(dir))
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,dir.data());
        root_directory_=dir;
    }
    void set_destination_directory(std::string_view dir){
        if(!fs::exists(dir) && !fs::create_directories(dir))
            throw std::invalid_argument("Unable tot create directory "s + dir.data());
        dest_directory_=dir;
    }
    void set_properties(std::optional<CommonDataProperties> common = {},
                        std::optional<long long> from_date = {},
                        std::optional<long long> to_date = {},
                        std::optional<TimeSeparation> t_sep= {},
                        std::optional<RepresentationType> grid_type = {},
                        std::optional<Coord> position = {}){
        props_ = Properties({common,from_date,to_date,t_sep,grid_type,position});
    }
    template<typename ARG>
    void set_common_data(ARG&& cmn_data){
        props_.common_.emplace(std::forward<ARG>(cmn_data));
    }
    template<typename ARG>
    void set_from_date(ARG&& from){
        props_.from_date_.emplace(std::forward<ARG>(from));
    }
    template<typename ARG>
    void set_to_date(ARG&& to){
        props_.to_date_.emplace(std::forward<ARG>(to));
    }
    void set_time_separation(TimeSeparation t_sep){
        props_.t_sep_.emplace(t_sep);
    }
    void set_grid_respresentation(RepresentationType grid_type){
        props_.grid_type_.emplace(grid_type);
    }
    template<typename ARG>
    void set_position(ARG&& pos){
        props_.position_.emplace(std::forward<ARG>(pos));
    }
    void set_using_processor_cores(int cores){
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    const GribDataInfo& get_result() const{
        return result;
    }
    void clear_result(){
        result.info_.clear();
        result.err = ErrorCodeData::NONE_ERR;
    }
    GribDataInfo&& release_result() noexcept{
        GribDataInfo res(std::move(result));
        return res;
    }
};