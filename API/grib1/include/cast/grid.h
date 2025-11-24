#pragma once
#include "sections/grid/grid.h"
#include "concepts.h"
#include <iostream>

std::string grid_to_txt(const std::ranges::range auto& grids) noexcept{
    return std::views::join_with(std::views::transform(grids,[](auto org){
        return std::string_view(grid_to_abbr(org));
    }),';')|std::ranges::to<std::string>();
}

std::vector<RepresentationType> approx_match_grid(const String auto& name) noexcept
{
    std::vector<RepresentationType> result;
    if(auto org = abbr_to_grid(name);org.has_value()){
        result.push_back(org.value());
        return result;
    }
    for(int i=0;i<256;++i)
        if(std::string_view(grid_to_text(static_cast<RepresentationType>(i))).contains(std::string_view(name)))
            result.push_back(static_cast<RepresentationType>(i));
        else continue;
    return result;
}

std::vector<RepresentationType> multitoken_approx_match_grid(const RangeOfStrings auto& tokens) noexcept
{
    std::vector<RepresentationType> found;
    bool first_check =false;
    for(std::string_view token:tokens)
        for(auto token_splitted:std::ranges::split_view(token,' ')){
            std::string_view token_string(token_splitted);
            std::cout<<token_string<<std::endl;
            if(first_check){
                if(found.empty())
                    return found;
                else{
                    std::vector<RepresentationType> tmp;
                    std::ranges::set_intersection(found,approx_match_grid(token_string),std::back_inserter(tmp));
                    found = std::move(tmp);
                }
            }
            else{
                found = approx_match_grid(token_string);            
                first_check = true;
            }
        }
    return found;
}