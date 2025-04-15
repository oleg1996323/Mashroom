#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "sys/error_print.h"
#include "types/data_info.h"
#include "message.h"
#include "def.h"

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
    std::set<int64_t> found;
    std::vector<ErrorFiles> err_files;
};

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::chrono_literals;
static constexpr const char* miss_files_filename = "missing_files.txt";
static constexpr const char* errorness_files_filename = "corrupted_files.txt";
class Check{
    private:
    Properties props_;
    GribDataInfo data_;
    fs::path root_directory_; //input root directory
    fs::path dest_directory_; //output log directory
    int cpus = 1;
    ProcessResult process_core(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print = nullptr);
    public:
    static bool check_format(std::string_view fmt);
    bool execute();
    void set_checking_directory(std::string_view dir){
        if(!fs::exists(dir))
            ErrorPrint::print_error(ErrorCode::DIRECTORY_X1_DONT_EXISTS,"",AT_ERROR_ACTION::ABORT,dir.data());
        if(!fs::is_directory(dir))
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,dir.data());
        root_directory_=dir;
    }
    void set_destination_directory(std::string_view dir){
        if(!fs::exists(dir) && !fs::create_directories(dir))
            ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,dir);
        else{
            if(!fs::is_directory(dir))
                ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir);
        }
        dest_directory_=dir;
    }
    void set_properties(std::optional<CommonDataProperties> common = CommonDataProperties(),
                        std::chrono::system_clock::time_point from_date = system_clock::time_point(sys_days(1970y/1/1)),
                        std::chrono::system_clock::time_point to_date = system_clock::now(),
                        TimeSeparation t_sep= TimeSeparation::DAY,
                        std::optional<RepresentationType> grid_type = {},
                        std::optional<Coord> position = {}){
        props_ = Properties({common,from_date,to_date,grid_type,position,t_sep});
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
        props_.t_sep_=t_sep;
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
    const GribDataInfo& data() const{
        return data_;
    }
    void clear_result(){
        data_.info_.clear();
        data_.err = ErrorCodeData::NONE_ERR;
    }
    GribDataInfo&& release_result() noexcept{
        GribDataInfo res(std::move(data_));
        return res;
    }
};