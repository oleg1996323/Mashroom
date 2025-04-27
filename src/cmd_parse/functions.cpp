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
std::vector<std::string> split(const std::string& str,const char* delimiter) noexcept{
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
std::vector<std::string_view> split(std::string_view str, const char* delimiter) noexcept{
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

std::optional<TimeOffset> get_time_offset_from_token(std::string_view input, ErrorCode& err){
    TimeOffset result;
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    if(!tokens.empty()){
        for(std::string_view token:tokens){
            auto tmp(from_chars<int>(token.substr(1),err));
            if(!tmp.has_value())
                return std::nullopt;
            else{
                if(tmp.value()<0){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid interval token input",AT_ERROR_ACTION::CONTINUE,token);
                    return std::nullopt;
                }
                else if(tmp.value()==0){
                    ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,token);
                    continue;
                }
            }
            if(token.size()>0){
                if(token.starts_with("h"))
                    result.hours_ = hours(tmp.value());
                else if(token.starts_with("y"))
                    result.years_ = years(tmp.value());
                else if(token.starts_with("m"))
                    result.months_ = months(tmp.value());
                else if(token.starts_with("d"))
                    result.days_ = days(tmp.value());
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown token for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,token);
                    return std::nullopt;
                }
            }
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
                return std::nullopt;
            }
        }
        return result;
    }
    else{
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
        return std::nullopt;
    }
}

std::optional<utc_tp> get_date_from_token(std::string_view input,ErrorCode& err){
    std::chrono::system_clock::time_point result = std::chrono::system_clock::time_point();
    std::chrono::year year_;
    std::chrono::month month_;
    std::chrono::day day_;
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    if(!tokens.empty()){
        for(std::string_view token:tokens){
            auto tmp(from_chars<int>(token.substr(1),err));
            if(!tmp.has_value())
                return std::nullopt;
            if(token.size()>0){
                if(token.starts_with("h")){
                    if(tmp.value()<0){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid year input",AT_ERROR_ACTION::CONTINUE,token);
                        err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                        return std::nullopt;
                    }
                    result += hh_mm_ss(hours(tmp.value())).to_duration();
                }
                else if(token.starts_with("y")){
                    if(tmp.value()<0){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid year input",AT_ERROR_ACTION::CONTINUE,token);
                        err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                        return std::nullopt;
                    }
                    year_ = std::chrono::year(tmp.value());
                }
                else if(token.starts_with("m")){
                    if(tmp.value()<=0){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid month input",AT_ERROR_ACTION::CONTINUE,token);
                        err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                        return std::nullopt;
                    }
                    month_ = std::chrono::month(tmp.value());
                }
                else if(token.starts_with("d")){
                    if(tmp.value()<=0){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid day input",AT_ERROR_ACTION::CONTINUE,token);
                        err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                        return std::nullopt;
                    }
                    day_ = std::chrono::day(tmp.value());
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown token for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,token);
                    err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return std::nullopt;
                }
            }
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
                err=ErrorCode::COMMAND_INPUT_X1_ERROR;
                return std::nullopt;
            }
        }
    }
    else{
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
        err=ErrorCode::COMMAND_INPUT_X1_ERROR;
        return std::nullopt;
    }
    year_month_day ymd(year_/month_/day_);
    if(!ymd.ok()){
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::CONTINUE,input);
        err=ErrorCode::INCORRECT_DATE;
        return std::nullopt;
    }
    return sys_days(ymd).time_since_epoch()+result;
}

std::vector<SearchParamTableVersion> post_parsing_parameters_aliases_def(Organization center,const std::vector<std::pair<uint8_t,std::string_view>>& alias_params_by_tab_ver,std::set<int>& error_pos){
    std::vector<SearchParamTableVersion> result;
    for(int i;i<alias_params_by_tab_ver.size();++i)
        error_pos.insert(i);

    int pos = 0;
    for(const auto& [table_version,parameter_alias]:alias_params_by_tab_ver){
        for(int i=0;i<256;++i){
            auto tab_ptr = parameter_table(center,table_version,(uint8_t)i);
            if(!tab_ptr)
                break;
            if(parameter_alias==tab_ptr->name){
                result.push_back({(uint8_t)i,table_version});
                error_pos.erase(pos);
                break;
            }
            else continue;
        }
        ++pos;
    }
    return result;
}

std::optional<Coord> get_coord_from_token(std::string_view input,ErrorCode& err){
    Coord result;
    std::vector<std::string_view> tokens = split(input,":");
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
            err = ErrorCode::COMMAND_INPUT_X1_ERROR;
            return std::nullopt;
        }
    return result;
}