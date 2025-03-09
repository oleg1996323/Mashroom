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

Date get_date_from_token(std::string_view input){
    Date result = Date();
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    if(!tokens.empty())
        for(std::string_view token:tokens){
            if(token.size()>0){
                if(token.starts_with("h")){
                    result.hour = from_chars<int>(token);
                }
                else if(token=="y"){
                    result.year = from_chars<int>(token);
                }
                else if(token=="m"){
                    result.month = from_chars<int>(token);
                }
                else if(token=="d"){
                    result.day = from_chars<int>(token);
                }
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,token);
            }
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
        }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
    return result;
}