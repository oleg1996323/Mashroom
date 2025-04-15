#include "cmd_parse/functions.h"
#include "error_print.h"
#include <iostream>

std::string get_string_mode(MODE mode)
{
    switch(mode){
        case(MODE::NONE):
            return "None mode";
        case(MODE::CAPITALIZE):
            return "Capitalize mode (arg = -cap)";
        case(MODE::CHECK_ALL_IN_PERIOD):
            return "Check mode (arg = -check)";
        case(MODE::EXTRACT):
            return "Extract mode (arg = -ext)";
        default:{
            std::cout<<"Argument error at \"get_string_mode\""<<std::endl;
            exit(1);
        }
    }
}
std::vector<std::string> split(const std::string& str,const char* delimiter){
    std::vector<std::string> result;
    size_t beg_pos = 0;
    size_t pos = beg_pos;
    while(true){
        pos=str.find_first_of(delimiter,pos);
        if(pos!=std::string::npos){
            result.push_back(str.substr(beg_pos,pos-beg_pos));
            beg_pos=pos;
        }
        else return result;
    }
}
std::vector<std::string_view> split(std::string_view str, const char* delimiter){
    std::vector<std::string_view> result;
    size_t beg_pos = 0;
    size_t pos = 0;
    while(pos!=std::string::npos){
        pos = str.find_first_of(delimiter,beg_pos);
        result.push_back(str.substr(beg_pos,pos-beg_pos));
        beg_pos=pos+1;
    }
    return result;
}

std::chrono::system_clock::time_point get_date_from_token(std::string_view input){
    std::chrono::system_clock::time_point result = std::chrono::system_clock::time_point();
    std::chrono::year year_;
    std::chrono::month month_;
    std::chrono::day day_;
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    if(!tokens.empty())
        for(std::string_view token:tokens){
            if(token.size()>0){
                if(token.starts_with("h")){
                    result += hh_mm_ss(hours(from_chars<int>(token.substr(1)))).to_duration();
                }
                else if(token.starts_with("y")){
                    year_ = std::chrono::year(from_chars<int>(token.substr(1)));
                }
                else if(token.starts_with("m")){
                    month_ = std::chrono::month(from_chars<int>(token.substr(1)));
                }
                else if(token.starts_with("d")){
                    day_ = std::chrono::day(from_chars<int>(token.substr(1)));
                }
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,token);
            }
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
        }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
    year_month_day ymd(year_/month_/day_);
    if(!ymd.ok())
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::ABORT,input);
    return sys_days();
}