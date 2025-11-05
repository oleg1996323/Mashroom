#pragma once
#include <string>
#include "types/time_period.h"
#include "sys/outputdatafileformats.h"
#include <format>

std::string generate_format_from_time_period(const TimePeriod& t_off) noexcept;

template <typename... ARGS>
std::string generate_directory_format(ARGS &&...args) noexcept;

template<typename... ARGS>
std::string generate_filename_format(ARGS&&... args);

std::string get_file_fmt(OutputDataFileFormats fmt) noexcept;

template<typename... ARGS>
std::string generate_filename_format(ARGS&&... args){
    std::string format;
    auto add_to_format = [&format](auto&& arg){
        if constexpr (std::is_same_v<TimePeriod,std::decay_t<decltype(arg)>>)
            format+=generate_format_from_time_period(arg);
        else format+="{}";
    };
    (add_to_format(args),...);
    return format;
}

template <typename... ARGS>
fs::path generate_filename(OutputDataFileFormats output_format,const std::string& format,ARGS &&...args)
{
    if(!format.empty()){
        return std::vformat(format + get_file_fmt(output_format), std::make_format_args(args...));
    }
    else throw std::invalid_argument("Invalid filename format");
    
}
template <typename... ARGS>
fs::path generate_directory(const fs::path& parent_path,const std::string& path_format,ARGS &&...args) noexcept{   
    using namespace std::string_literals;
    if(path_format.empty()){
        std::string path_format_tmp;
        auto add_fmt_arg = [&path_format_tmp](auto&& arg){
            if constexpr(std::is_same_v<std::decay_t<decltype(arg)>,TimePeriod>){
                if (arg.seconds_ > std::chrono::seconds(0))
                    path_format_tmp = path_format_tmp + "S%" + fs::path::preferred_separator+
                    "M%" + fs::path::preferred_separator + 
                    "H%" + fs::path::preferred_separator;
                else if (arg.minutes_ > minutes(0))
                    path_format_tmp = path_format_tmp + "M%" + fs::path::preferred_separator + "H%" + fs::path::preferred_separator;
                else if (arg.hours_ > hours(0))

                    path_format_tmp = path_format_tmp + "H%" + fs::path::preferred_separator;
                if (arg.days_ > days(0))
                    path_format_tmp = path_format_tmp + "d%" + fs::path::preferred_separator +
                    "m%" + fs::path::preferred_separator;
                else if (arg.months_ > months(0))
                    path_format_tmp = path_format_tmp + "m%" + fs::path::preferred_separator;
                path_format_tmp = path_format_tmp + "Y%";
                std::reverse(path_format_tmp.begin(), path_format_tmp.end());
                path_format_tmp = (path_format_tmp.empty()?"":"_")+"{:"s + path_format_tmp + "}"s;
            }
            else{
                path_format_tmp+=(path_format_tmp.empty()?""s:"_"s)+"{}"s;
            }
        };
        (add_fmt_arg(args),...);
    }
    return parent_path / std::vformat(path_format, std::make_format_args(args...));
}

template <typename... ARGS>
std::string generate_directory_format(ARGS &&...args) noexcept{
    static_assert(sizeof...(args)>0); 
    using namespace std::string_literals;
    std::string path_format_tmp;
    auto add_fmt_arg = [&path_format_tmp](auto&& arg){
        if constexpr(std::is_same_v<std::decay_t<decltype(arg)>,TimePeriod>){
            if (arg.seconds_ > std::chrono::seconds(0))
                path_format_tmp = path_format_tmp + "S%" + fs::path::preferred_separator+
                "M%" + fs::path::preferred_separator + 
                "H%" + fs::path::preferred_separator;
            else if (arg.minutes_ > minutes(0))
                path_format_tmp = path_format_tmp + "M%" + fs::path::preferred_separator + "H%" + fs::path::preferred_separator;
            else if (arg.hours_ > hours(0))

                path_format_tmp = path_format_tmp + "H%" + fs::path::preferred_separator;
            if (arg.days_ > days(0))
                path_format_tmp = path_format_tmp + "d%" + fs::path::preferred_separator +
                "m%" + fs::path::preferred_separator;
            else if (arg.months_ > months(0))
                path_format_tmp = path_format_tmp + "m%" + fs::path::preferred_separator;
            path_format_tmp = path_format_tmp + "Y%";
            std::reverse(path_format_tmp.begin(), path_format_tmp.end());  
            path_format_tmp="{:"+path_format_tmp+"}";      
        }
        else{
            path_format_tmp="{"+path_format_tmp+fs::path::preferred_separator+"}";
        }
    };
    (add_fmt_arg(args),...);
    return path_format_tmp;
}