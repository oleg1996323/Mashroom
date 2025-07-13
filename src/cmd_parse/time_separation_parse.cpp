#include "cmd_parse/time_separation_parse.h"

std::optional<TimeOffset> get_time_offset_from_token(std::string_view input, ErrorCode& err){
    TimeOffset result;
    std::vector<std::string_view> tokens = split<std::string_view>(std::string_view(input),":");
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

ErrorCode time_separation_parse(std::string_view arg, AbstractTimeSeparation& obj){
    ErrorCode err = ErrorCode::NONE;
    auto interval = get_time_offset_from_token(arg,err);
    if(!interval.has_value())
        return err;
    obj.set_offset_time_interval(interval.value());
    return ErrorCode::NONE;
}
std::string_view commands_from_time_separation(std::string_view arg,ErrorCode& err){
    if(!get_time_offset_from_token(arg,err).has_value())
        return {};
    else return arg;
}

namespace boost::program_options{

void validate(boost::any& v,
              const std::vector<std::string>& values,
              utc_tp* target_type, int)
{
    validators::check_first_occurrence(v);
    v = lexical_cast<utc_tp>(validators::get_single_string(values));
}

}

template<>
utc_tp boost::lexical_cast(const std::string& input){
    std::istringstream s(input);
    s.imbue(std::locale("en_US.utf-8"));
    utc_tp tp;
    s>>std::chrono::parse("%d/%m/%y-%T",tp);
    if(s.fail())
        throw boost::bad_lexical_cast();
    return tp;
}