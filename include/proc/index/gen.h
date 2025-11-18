#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "filesystem.h"
#include "proc/index/indexdatafileformat.h"
#include "types/time_interval.h"
#include "sections/grid/grid.h"
#include "proc/index/indexoutputfileformat.h"
#include "name_tokens.h"

namespace index_gen{

const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>>& token_to_txts() noexcept;
const std::unordered_map<std::string,DIR_TREE_TOKEN>& txt_to_tokens() noexcept;
std::vector<DIR_TREE_TOKEN> input_to_token_sequence(const std::string&);
bool check_format(std::string_view fmt);
bool check_format(const std::vector<DIR_TREE_TOKEN>& tokens);
std::string format(DIR_TREE_TOKEN token);

std::string generate_filename_format(std::vector<DIR_TREE_TOKEN> tokens) noexcept;

template <typename ARGS>
std::string generate_formatted(ARGS&& arg){
    using namespace std::string_literals;
    using type = std::decay_t<decltype(arg)>;
    if constexpr(std::is_same_v<type,year>){
        return std::vformat(format(DIR_TREE_TOKEN::YEAR),std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,year_month>){
        auto year = arg.year();
        auto month = arg.month();
        return std::vformat(format(DIR_TREE_TOKEN::YEAR),std::make_format_args(year));    
        return std::vformat(format(DIR_TREE_TOKEN::MONTH),std::make_format_args(month));
    }
    else if constexpr(std::is_same_v<type,year_month_day>){
        auto year = arg.year();
        auto month = arg.month();
        auto day = arg.day();
        return std::vformat(format(DIR_TREE_TOKEN::YEAR),std::make_format_args(year));    
        return std::vformat(format(DIR_TREE_TOKEN::MONTH),std::make_format_args(month));  
        return std::vformat(format(DIR_TREE_TOKEN::DAY),std::make_format_args(day));             
    }
    else if constexpr(std::is_same_v<type,utc_tp>){
        auto ymd = year_month_day(floor<days>(arg));
        auto year = ymd.year();
        auto month = ymd.month();
        auto day = ymd.day();
        auto hour = floor<hours>(arg);
        return std::vformat(format(DIR_TREE_TOKEN::YEAR),std::make_format_args(year))+    
        std::vformat(format(DIR_TREE_TOKEN::MONTH),std::make_format_args(month)) +
        std::vformat(format(DIR_TREE_TOKEN::DAY),std::make_format_args(day)) +
        std::vformat(format(DIR_TREE_TOKEN::HOUR),std::make_format_args(hour));          
    }
    else if constexpr(std::is_same_v<type,month>){
        return std::vformat(format(DIR_TREE_TOKEN::MONTH),std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,day>){
        return std::vformat(format(DIR_TREE_TOKEN::DAY),std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,hours>){
        return std::vformat(format(DIR_TREE_TOKEN::HOUR),std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,Lat>){
        return std::vformat(format(DIR_TREE_TOKEN::LATITUDE),std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,Lon>){
        return std::vformat(format(DIR_TREE_TOKEN::LONGITUDE),std::make_format_args(arg));                
    }
    else if constexpr(std::is_integral_v<type>){
        return std::vformat("{}",std::make_format_args(arg));                
    }        
    else if constexpr(std::is_same_v<type,std::string>){
        return std::vformat("{}",std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,const char*>){
        return std::vformat("{}",std::make_format_args(arg));                
    }
    else if constexpr(std::is_same_v<type,std::string_view>){
        return std::vformat("{}",std::make_format_args(arg));                
    }
    else static_assert(false,"filename format argument");
}

template <typename... ARGS>
std::string generate_fs(std::string separator,ARGS&&... args){
    static_assert(sizeof...(args)>0);
    std::vector<std::string> parts = {generate_formatted(std::forward<ARGS>(args))...};
    if(parts.empty())
        throw std::invalid_argument("Invalid generated string");
    else return std::views::join_with(parts,separator) | std::ranges::to<std::string>();
}

template <typename... ARGS>
fs::path generate_filename(IndexOutputFileFormat::token output_format,ARGS &&...args){
    using namespace std::string_literals;
    return generate_fs("_"s,std::forward<ARGS>(args)...)+output_index_token_to_extension(output_format);
}

template <typename... ARGS>
fs::path generate_directories_path(ARGS &&...args){
    return generate_fs(fs::path::preferred_separator,std::forward<ARGS>(args)...);
}

std::string generate_directory_format(const std::vector<DIR_TREE_TOKEN>& tokens) noexcept;
}
