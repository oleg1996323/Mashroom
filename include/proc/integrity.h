#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "grib1/message.h"
#include "OsterLib/types/time_period.h"
#include "data/datastruct.h"

using namespace std::chrono_literals;
namespace fs = std::filesystem;

template<class Clock, class Duration> 
struct std::incrementable_traits<std::chrono::time_point<Clock, Duration>> { 
  using difference_type = typename Duration::rep;
};

struct ErrorFiles{
    fs::path name;
    api::errc<API_T::GRIB1> code;
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
    int cpus = 1;
    std::pair<std::unordered_set<DataStructVariation>,std::vector<std::pair<Location<false>,osterlib::ContextedError>>>  
    __check_file_data_integrity__(const std::vector<fs::directory_entry>&,std::mutex*) noexcept;
    osterlib::ContextedError __check_metadata_integrity__(const std::unordered_set<DataStructVariation>&,std::mutex*) noexcept;
    osterlib::ContextedError __correct_indexation__(const std::unordered_set<DataStructVariation>&) noexcept;
    
    public:
    virtual osterlib::ContextedError execute() noexcept override final;
    virtual osterlib::ContextedError properties_integrity() const noexcept override final{
        if( props_.from_date_.has_value() &&
            props_.to_date_.has_value() && 
            is_correct_interval(*props_.from_date_,*props_.to_date_))
        {
            osterlib::ContextedError ctx_error(mashroom::errc::invalid_argument);
            ctx_error.with_field("procedure","integrity")
            .with_field("at","properties_integrity")
            .with_field("arg","date-time interval");
            return ctx_error;
        }
        if(!props_.position_.has_value())
        {
            osterlib::ContextedError ctx_error(mashroom::errc::undefined_value);
            ctx_error.with_field("procedure","integrity")
            .with_field("at","properties_integrity")
            .with_field("value","coordinate");
            return ctx_error;
        }
        if(!is_correct_pos(&props_.position_.value())) //actually for WGS84
        {
            osterlib::ContextedError ctx_error(mashroom::errc::invalid_argument);
            ctx_error.with_field("procedure","integrity")
            .with_field("at","properties_integrity")
            .with_field("arg","coordinate");
            return ctx_error;
        }
        return {};
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