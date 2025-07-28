#pragma once
#include "code_tables/table_0.h"
#include "paramtablev.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <parsing.h>
#include <ranges>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "concepts.h"
#include "boost/algorithm/algorithm.hpp"

std::string parameters_to_txt(Organization center,const std::ranges::range auto& parameters) noexcept requires(
    std::is_same_v<std::ranges::range_value_t<std::decay_t<decltype(parameters)>>,SearchParamTableVersion>
)
{
    return std::views::join_with(std::views::transform(parameters,[&](auto org){
        return std::string_view(parameter_table(center,org.t_ver_,org.param_)->name);
    }),' ')|std::ranges::to<std::string>();
}

std::vector<SearchParamTableVersion> approx_match_parameter(Organization center, const String auto& token) noexcept
{
    std::vector<SearchParamTableVersion> result;
    for(int table_version=0;table_version<256;++table_version)
        for(int param=0;param<256;++param){
            if(auto ptr = parameter_table(center,table_version,param);ptr!=nullptr){
                std::string_view to_check(ptr->comment);
                if(boost::iequals(ptr->name,token))
                    result.push_back(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_version)});
                else if(boost::ifind_first(to_check,std::string_view(token)))
                    result.push_back(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_version)});
                else continue;
            }
            else continue;
        }
    return result;
}

std::vector<SearchParamTableVersion> multitoken_approx_match_parameter(Organization center, const RangeOfStrings auto& tokens) noexcept
{
    std::vector<SearchParamTableVersion> found;
    bool first_check =false;
    for(auto token:tokens)
        if(first_check){
            if(found.empty())
                return found;
            else{
                if(token.empty())
                    continue;
                std::vector<SearchParamTableVersion> tmp;
                auto f = approx_match_parameter(center,token);
                std::cout<<"Found: "<<parameters_to_txt(center,f)<<std::endl;
                std::ranges::set_intersection(found,approx_match_parameter(center,token),std::back_inserter(tmp),std::less<SearchParamTableVersion>());
                found = std::move(tmp);
            }
        }
        else{
            if(token.empty())
                continue;
            found = approx_match_parameter(center,token);
            std::cout<<"Found: "<<parameters_to_txt(center,found)<<std::endl;        
            first_check = true;
        }
    return found;
}