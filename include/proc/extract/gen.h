#pragma once
#include <string>
#include "types/time_period.h"
#include "sys/outputdatafileformats.h"
#include <format>
#include "types/time_interval.h"

#define VERSION int(0<<16|0<<8|1)

template<typename T>
requires (IsTimePoint<T> || std::is_same_v<T,std::chrono::year> || std::is_same_v<T,std::chrono::year_month>)
std::string generate_format_from_time(const T& time,char separator) noexcept{
    using namespace std::string_literals;
    if constexpr (IsTimePoint<T>){
        if constexpr (std::is_same_v<typename T::duration,std::chrono::minutes>)
            return "{:%Y"s+ separator+"%m" + separator+"%d" + separator+"%H"+
            separator+"%M}";
        else if constexpr (std::is_same_v<typename T::duration,std::chrono::hours>)
            return "{:%Y"s+ separator+"%m" + separator+"%d" + separator+"%H}";
        else if constexpr (std::is_same_v<typename T::duration,std::chrono::days>)
            return "{:%Y"s+ separator+"%m" + separator+"%d}";
        else return "{:%Y"s+ separator+"%m" + separator+"%d" + separator+"%H"+
            separator+"%M"+separator+"%S}";
    }
    else if constexpr (std::is_same_v<T,std::chrono::year_month>)
        return "{:%Y"s+ separator+"%m}";
    else if constexpr (std::is_same_v<T,std::chrono::year>)
        return "{:%Y}"s;
    else static_assert(false,"Invalid use of formatting from time (acceptable types time_point<seconds/minutes/hours/days>, year_month, year)");
}

using TimePeriodRounded = std::variant<year,year_month,utc_tp_t<days>,utc_tp_t<hours>,utc_tp_t<minutes>,utc_tp_t<std::chrono::seconds>>;

TimePeriodRounded round_by_time_diff(const DateTimeDiff& diff, utc_tp_t<std::chrono::seconds> tp) {
    
    if(diff.years_>0)
        return year_month_day(std::chrono::floor<std::chrono::days>(tp)).year();
    else if(diff.months_>0){
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(tp));
        return year_month(ymd.year(),ymd.month());
    }
    else if(diff.days_>0)
        return std::chrono::floor<std::chrono::days>(tp);
    else if(diff.hours_>0)
        return std::chrono::floor<std::chrono::hours>(tp);
    else if(diff.minutes_>0)
        return std::chrono::floor<std::chrono::minutes>(tp);
    else if(diff.seconds_>0)
        return std::chrono::floor<std::chrono::seconds>(tp);
    else{
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(tp));
        return year_month(ymd.year(),ymd.month());
    }
}

std::string get_file_fmt(OutputDataFileFormats fmt) noexcept;

namespace generators::helpers{
    struct FilenameAddFmtArg {
        fs::path& fn;
        template<typename T>
        void operator()(const T& arg){
            using arg_type = std::decay_t<T>;
            if(!fn.empty())
                fn+='_';
            if constexpr(IsTimePoint<arg_type> || std::is_same_v<arg_type,std::chrono::year_month> || std::is_same_v<arg_type,std::chrono::year>)
                fn += std::vformat(generate_format_from_time(arg, '_').c_str(), std::make_format_args(arg));
            else if constexpr (IsStdVariant<arg_type>) {
                std::visit(FilenameAddFmtArg{fn}, arg);
            }
            else {
                fn += std::format("{}", arg);
            }
        }
    };

    struct DirectoryAddFmtArg {
        fs::path& dir;
        template<typename T>
        void operator()(const T& arg){
            using arg_type = std::decay_t<T>;
            if constexpr(IsTimePoint<arg_type>)
                dir /= std::vformat(generate_format_from_time(arg, fs::path::preferred_separator).c_str(), std::make_format_args(arg));
            else if constexpr (IsStdVariant<arg_type>) {
                std::visit(DirectoryAddFmtArg{dir}, arg);
            }
            else {
                dir /= std::format("{}", arg);
            }
        }
    };
}

template <typename... ARGS>
fs::path generate_filename(OutputDataFileFormats output_format,ARGS&&... args)
{   
    fs::path fn;
    generators::helpers::FilenameAddFmtArg add_fmt_arg{fn};
    (add_fmt_arg(args), ...);
    return fn.string()+get_file_fmt(output_format);
}

template <typename... ARGS>
fs::path generate_directory(const fs::path& parent_path,ARGS&&...args) noexcept{   
    fs::path dir = parent_path;
    generators::helpers::DirectoryAddFmtArg add_fmt_arg{dir};
    (add_fmt_arg(std::forward<ARGS>(args)), ...);
    return dir;
}