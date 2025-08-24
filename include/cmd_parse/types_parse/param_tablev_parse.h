#pragma once
#include "definitions/def.h"
#include <boost/program_options.hpp>
#include <vector>
#include <string_view>
#include <string>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <expected>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "concepts.h"
#include "cast/parameters.h"
#include "cmd_parse/functions.h"

struct SearchParamTableVersion;

namespace parse::parameter_tv{
std::unordered_set<SearchParamTableVersion> param_by_tv_abbr(Organization center,const RangeOfStrings auto& input){
    std::vector<std::vector<std::string>> string_tokens;
    std::unordered_set<SearchParamTableVersion> result;
    for(const auto& string:input)
        string_tokens.push_back(std::views::split(string,' ')|std::ranges::to<std::vector<std::string>>());
    for(const auto& tokens:string_tokens){
        std::vector<SearchParamTableVersion> parameters =  multitoken_approx_match_parameter(center,tokens);
        if(parameters.empty()){
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "not matched parameters",AT_ERROR_ACTION::CONTINUE,std::ranges::join_with_view(tokens," ")|std::ranges::to<std::string>());
            continue;
        }
        else{
            //std::cout<<"Matched more than 1 parameters:"<<std::endl;
            //std::cout<<parameters_to_txt(center,parameters)<<std::endl;
            result.insert(std::make_move_iterator(parameters.begin()),std::make_move_iterator(parameters.end()));
        }
    }
    return result;
}
}