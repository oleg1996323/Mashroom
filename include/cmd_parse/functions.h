#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <charconv>
#include <stdexcept>

#ifdef __cplusplus
extern "C"{
    #include "PDSdate.h"
    #include "coords.h"
    #include "func.h"
}
#endif
#include "cmd_parse/cmd_def.h"
#include "error_print.h"

template <class T, class... Args>
T from_chars(std::string_view s, Args... args){
    const char *end = s.data() + s.size();
    T number;
    auto result = std::from_chars(s.data(), end, number, args...);
    if (result.ec != std::errc{} || result.ptr != end)
        throw std::runtime_error("Cannot convert to number");
    return number;
}
std::string get_string_mode(MODE mode);
std::vector<std::string> split(const std::string& str,const char* delim);
std::vector<std::string_view> split(std::string_view str,const char* delim);
Date get_date_from_token(std::string_view token);

template<typename T>
T get_coord_from_token(std::string_view input, DataExtractMode& mode){
    if constexpr(std::is_same_v<T,Coord>){
        if(mode == DataExtractMode::RECT)
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Conflict between arguments. Already choosen extraction mode by coordinate position.",
                AT_ERROR_ACTION::ABORT,input);
        else mode = DataExtractMode::POSITION;
        Coord result;
        std::vector<std::string_view> tokens = split(input,":");
        if(!tokens.empty())
            try{
                if(tokens.size()==2){
                    result.lat_ = from_chars<double>(tokens[0]);
                    result.lon_ = from_chars<double>(tokens[1]);
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Error at lat-lon input",AT_ERROR_ACTION::ABORT,input);
                }   
            }
            catch(const std::runtime_error& err){
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Error at lat-lon input",AT_ERROR_ACTION::ABORT,input);
            }
        return result;
    }
    else if(std::is_same_v<T,double>){
        if(mode == DataExtractMode::POSITION)
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Conflict between arguments. Already choosen extraction mode by rectangle.",
                AT_ERROR_ACTION::ABORT,input);
        else mode = DataExtractMode::RECT;
        return from_chars<double>(input);
    }
    //else static_assert(1);
}
