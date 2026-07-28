#pragma once
#include "definitions/def.h"
#include <vector>
#include <string_view>
#include <string>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <expected>
#include "sys/error.h"
#include "OsterLib/concepts.h"
#include "grib1/cast/parameters.h"

struct SearchParamTableVersion;

namespace parse::parameter_tv{
std::unordered_set<SearchParamTableVersion> param_by_tv_abbr(Organization center,const RangeOfStrings auto& input){
    std::vector<std::vector<std::string>> string_tokens;
    std::unordered_set<SearchParamTableVersion> result;
    osterlib::ContextedError ctx_error;
    for(const auto& string:input)
        string_tokens.push_back(std::views::split(string,' ')|std::ranges::to<std::vector<std::string>>());
    for(const auto& tokens:string_tokens){
        std::vector<SearchParamTableVersion> parameters =  multitoken_approx_match_parameter(center,tokens);
        if(parameters.empty()){
            ctx_error.
            ctx_error.with_context("not matched parameters");
            ErrorPrint::print_error(mashroom::errc::COMMAND_INPUT_X1_ERROR,
                    "not matched parameters",AT_ERROR_ACTION::CONTINUE,std::ranges::join_with_view(tokens," ")|std::ranges::to<std::string>());
            continue;
        }
        else
            result.insert(std::make_move_iterator(parameters.begin()),std::make_move_iterator(parameters.end()));
    }
    return result;
}
}