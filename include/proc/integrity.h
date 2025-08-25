#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "sys/error_print.h"
#include "data/sublimed_info.h"
#include "data/info.h"
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
    std::set<int64_t> found;
    std::vector<ErrorFiles> err_files;
};

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::chrono_literals;
static constexpr const char* missed_data = "missed.txt";
static constexpr const char* access_data = "accessible.txt";
static constexpr const char* errorness_files_filename = "corrupted_files.txt";

#include "proc/interfaces/abstractsearchprocess.h"
class Integrity:public AbstractSearchProcess{
    private:
    GribDataInfo data_;
    TimePeriod t_off_;
    std::vector<std::pair<fs::path,ErrorCodeData>> file_errors_;
    std::string time_result_format = "{:%Y/%m}";
    int cpus = 1;
    ErrorCode __process_core__(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print = nullptr) noexcept;
    
    public:
    virtual ErrorCode execute() noexcept override final;
    virtual ErrorCode properties_integrity() const noexcept override final{
        if(is_correct_interval(props_.from_date_,props_.to_date_))
            return ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date interval is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
        if(!props_.position_.has_value())
            return ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"Not defined",AT_ERROR_ACTION::CONTINUE);
        if(!is_correct_pos(&props_.position_.value())) //actually for WGS84
            return ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }
    void get_time_format() noexcept{
        std::string time_format_tmp;
        if(t_off_.hours_>std::chrono::seconds(0)){
            time_format_tmp+="S%:M%:H% ";
        }
        else if(t_off_.hours_>minutes(0)){
            time_format_tmp+="M%:H% ";
        }
        else if(t_off_.hours_>hours(0)){
            time_format_tmp+="H% ";
        }
        if(t_off_.days_>days(0)){
            time_format_tmp+="d%/m%/";
        }
        else if(t_off_.months_>months(0)){
            time_format_tmp+="m%/";
        }
        time_format_tmp+="Y%";
        std::reverse(time_format_tmp.begin(),time_format_tmp.end());
        time_format_tmp="{:"+time_format_tmp+"}";
    }

    void clear_result(){
        data_.info_.clear();
        data_.err = ErrorCodeData::NONE_ERR;
    }
    GribDataInfo release_result() noexcept{
        GribDataInfo res(std::move(data_));
        return res;
    }
};