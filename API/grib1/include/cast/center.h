#pragma once
#include "code_tables/table_0.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <parsing.h>
#include <ranges>
#include <iostream>
#include "concepts.h"

std::string centers_to_txt(const std::ranges::range auto& centers) noexcept requires(
    std::is_same_v<std::ranges::range_value_t<std::decay_t<decltype(centers)>>,Organization>
)
{
    return std::views::join_with(std::views::transform(centers,[](auto org){
        return std::string_view(center_to_abbr(org));
    }),';')|std::ranges::to<std::string>();
}

std::vector<Organization> approx_match_center(const String auto& name) noexcept
{
    std::vector<Organization> result;
    if(auto org = abbr_to_center(name);org.has_value()){
        result.push_back(org.value());
        return result;
    }
    for(int i=0;i<256;++i)
        if(std::string_view(center_to_text(static_cast<Organization>(i))).contains(std::string_view(name)))
            result.push_back(static_cast<Organization>(i));
        else continue;
    return result;
}

std::vector<Organization> multitoken_approx_match_center(const RangeOfStrings auto& tokens) noexcept
{
    std::vector<Organization> found;
    bool first_check =false;
    for(std::string_view token:tokens)
        if(first_check){
            if(found.empty())
                return found;
            else{
                std::vector<Organization> tmp;
                std::ranges::set_intersection(found,approx_match_center(token),std::back_inserter(tmp));
                found = std::move(tmp);
            }
        }
        else{
            found = approx_match_center(token);            
            first_check = true;
        }
    return found;
}