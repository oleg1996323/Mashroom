#include "cmd_parse/time_separation_parse.h"

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
TimeOffset boost::lexical_cast(const std::string& input){
    TimeOffset result;
    ErrorCode err;
    std::vector<std::string_view> tokens = split<std::string_view>(std::string_view(input),":");
    if(!tokens.empty()){
        for(std::string_view token:tokens){
            auto tmp(from_chars<int>(token.substr(1),err));
            if(!tmp.has_value())
                throw boost::bad_lexical_cast();
            else{
                if(tmp.value()<0){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid interval token input",AT_ERROR_ACTION::CONTINUE,token);
                    throw boost::bad_lexical_cast();
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
                    throw boost::bad_lexical_cast();
                }
            }
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
                throw boost::bad_lexical_cast();
            }
        }
        return result;
    }
    else{
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::CONTINUE,input);
        throw boost::bad_lexical_cast();
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