#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "sys/error_print.h"
#include "message.h"
#include "types/time_period.h"
#include "data/datastruct.h"

using namespace std::chrono_literals;
namespace fs = std::filesystem;

template<class Clock, class Duration> 
struct std::incrementable_traits<std::chrono::time_point<Clock, Duration>> { 
  using difference_type = typename Duration::rep;
};

struct ErrorFiles{
    fs::path name;
    API::ErrorData::Code<API::GRIB1> code;
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
    TimePeriod t_off_;
    std::vector<std::pair<fs::path,API::ErrorData::Code<API::GRIB1>::value>> file_errors_;
    int cpus = 1;
    DataStructVariation __check_file_data_integrity__(std::ranges::random_access_range auto&& entries,ErrorCode& err, std::mutex* mute_at_print = nullptr) noexcept;
    void __check_metadata_integrity__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err,std::mutex* mute_at_print = nullptr);
    void __correct_indexation__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err);
    
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
};