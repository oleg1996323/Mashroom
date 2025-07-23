#include "cmd_parse/param_tablev_parse.h"
#include "cmd_parse/functions.h"
#include <string_view>
#include "sections/section_1.h"
#include "code_tables/table_0.h"

template<>
void ::boost::program_options::validate(boost::any& v,const std::vector<std::string>& values,
                                        std::vector<SearchParamTableVersion>* target_type,int)
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(values);
    std::vector<SearchParamTableVersion> result;
    for(auto& s:values){
        auto parameter_tv = split<std::string_view>(s,":");
        if(parameter_tv.size()==2){
            auto table_version = from_chars<int>(parameter_tv.at(0));
            if(table_version.has_value()){
                auto parameter = from_chars<int>(parameter_tv.at(1));
                if(parameter.has_value()){
                    if(parameter.value()<0 || table_version.value()<0)
                        throw po::validation_error(po::validation_error::invalid_option_value,s);
                    else result.push_back(SearchParamTableVersion{ .param_=(uint8_t)parameter.value(),
                        .t_ver_=(uint8_t)table_version.value()});
                }
                else
                    throw po::validation_error(po::validation_error::invalid_option_value,s);
            }
            else throw po::validation_error(po::validation_error::invalid_option_value,s);
        }
        else throw po::validation_error(po::validation_error::invalid_option_value,s);
    }
    v = std::move(result);
}

std::expected<SearchParamTableVersion,ErrorCode> parse::parameter_tv::param_by_tv_abbr(Organization center,std::string_view input){
    auto tv_abbr = split<std::string_view>(input,":");
    if(tv_abbr.size()==2){
        auto table_version = from_chars<int>(tv_abbr.at(0));
        if(table_version.has_value()){
            for(int i=0;i<256;++i){
                auto tab_ptr = parameter_table(center,
                table_version.value(),(uint8_t)i);
                if(!tab_ptr)
                    continue;
                if(tv_abbr.at(1)==tab_ptr->name){
                    return SearchParamTableVersion{.param_ = (uint8_t)i,
                        .t_ver_=static_cast<uint8_t>(table_version.value())};
                }
                else continue;
            }
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Unknown parameter's abbreviation",AT_ERROR_ACTION::CONTINUE,tv_abbr.at(1)));
        }
        else return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Invalid table version input (must be integer, >=0 and <256)",AT_ERROR_ACTION::CONTINUE,input));
    }
    else return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "Invalid input: must be int:name, where 0<=int<256 and parameter name (not empty string)",AT_ERROR_ACTION::CONTINUE,input));
}