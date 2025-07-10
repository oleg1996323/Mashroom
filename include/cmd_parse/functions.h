#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <charconv>
#include <stdexcept>

#include "data/PDSdate.h"
#include "types/coord.h"
#include "cmd_parse/cmd_def.h"
#include "sys/error_print.h"
#include "types/time_interval.h"
#include "include/def.h"
#include <set>

template <class T, class... Args>
std::optional<T> from_chars(std::string_view s,ErrorCode& err, Args... args) noexcept{
    const char *end = s.data() + s.size();
    T number;
    auto result = std::from_chars(s.data(), end, number, args...);
    if (result.ec != std::errc{} || result.ptr != end){
        err = ErrorCode::COMMAND_INPUT_X1_ERROR;
        return std::nullopt;
    }
    return number;
}
std::string get_string_mode(MODE mode);
template<typename T>
std::vector<T> split(std::string_view str, const char* delimiter) noexcept{
    std::vector<T> result;
    size_t beg_pos = 0;
    size_t pos = 0;
    while(pos!=std::string::npos){
        pos = str.find_first_of(delimiter,beg_pos);
        result.push_back(str.substr(beg_pos,pos-beg_pos));
        beg_pos=pos+1;
    }
    return result;
}
std::optional<TimeOffset> get_time_offset_from_token(std::string_view token, ErrorCode& err);
std::optional<utc_tp> get_date_from_token(std::string_view token,ErrorCode& err);
std::vector<SearchParamTableVersion> post_parsing_parameters_aliases_def(Organization center,const std::vector<std::pair<uint8_t,std::string_view>>& alias_params_by_tab_ver,std::set<int>& error_pos);

template<typename T>
std::optional<T> get_coord_from_token(std::string_view input, ErrorCode& err, DataExtractMode& mode){
    if constexpr(std::is_same_v<T,Coord>){
        if(mode == DataExtractMode::RECT){
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Conflict between arguments. Already choosen extraction mode by coordinate position.",
                AT_ERROR_ACTION::ABORT,input);
            return std::nullopt;
        }
        else mode = DataExtractMode::POSITION;
        Coord result;
        std::vector<std::string_view> tokens = split<std::string_view>(input,":");
        if(!tokens.empty())
            if(tokens.size()==2){
                auto lat = from_chars<double>(tokens[0],err);
                if(!lat.has_value())
                    return std::nullopt;
                auto lon = from_chars<double>(tokens[1],err);
                if(!lon.has_value())
                    return std::nullopt;
                result.lat_ = lat.value();
                result.lon_ = lon.value();
            }
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Error at lat-lon input",AT_ERROR_ACTION::CONTINUE,input);
                return std::nullopt;
            }
        return result;
    }
    else if(std::is_same_v<T,double>){
        if(mode == DataExtractMode::POSITION){
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Conflict between arguments. Already choosen extraction mode by rectangle.",
                AT_ERROR_ACTION::CONTINUE,input);
            return std::nullopt;
        }
        else mode = DataExtractMode::RECT;
        return from_chars<double>(input,err);
    }
    else{
        assert(true);//incorrect type;
    }
}

std::optional<Coord> get_coord_from_token(std::string_view input,ErrorCode& err);