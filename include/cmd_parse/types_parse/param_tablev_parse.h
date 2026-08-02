#pragma once
#include "definitions/def.h"
#include <vector>
#include <string_view>
#include <string>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_set>
#include "sys/error.h"
#include "OsterLib/concepts.h"
#include "grib1/cast/parameters.h"

struct SearchParamTableVersion;

namespace parse::parameter_tv{
std::unordered_set<SearchParamTableVersion> param_by_tv_abbr(Organization center,const RangeOfStrings auto& input,osterlib::ContextedError& ctx_error){
    ctx_error.clear();
    std::vector<std::vector<std::string>> string_tokens;
    std::unordered_set<SearchParamTableVersion> result;
    for(const auto& string:input)
        string_tokens.push_back(std::views::split(string,' ')|std::ranges::to<std::vector<std::string>>());
    for(const auto& tokens:string_tokens){
        std::vector<SearchParamTableVersion> parameters =  multitoken_approx_match_parameter(center,tokens);
        if(parameters.empty()){
            if(ctx_error)
                ctx_error.error(mashroom::errc::command_input_error,"not matched parameters");
            ctx_error.with_field("at","get parameters by table-version abbreviation");
            for(auto& token:tokens)
                ctx_error.with_field("token",token);
            continue;
        }
        else
            result.insert(std::make_move_iterator(parameters.begin()),std::make_move_iterator(parameters.end()));
    }
    return result;
}
}