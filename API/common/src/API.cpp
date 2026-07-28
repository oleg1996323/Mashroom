#include "common/API.h"
#include <flat_map>

template<>
struct std::less<API_T>{
    bool operator()(API_T lhs,API_T rhs) const noexcept{
        return lhs<rhs;
    }
};

const std::array<std::pair<API_T, std::string_view>, 5> type_names = {
    std::pair{API_T::COMMON, "common"},
    #ifdef GRIB1API
        std::pair{API_T::GRIB1, "grib v1"},
    #endif
    #ifdef GRIB2API
        std::pair{API_T::GRIB2, "grib v2"},
    #endif
    #ifdef GRIB3API
        std::pair{API_T::GRIB3, "grib v3"},
    #endif
    #ifdef NETCDFAPI
        std::pair{API_T::NETCDF, "netcdf"},
    #endif
};

std::flat_map<API_T, std::string_view> names_by_types(type_names.begin(), type_names.end());

std::flat_map<std::string_view,API_T> types_by_names=
    [](const std::flat_map<API_T,std::string_view>& reversed){
        std::flat_map<std::string_view,API_T> result;
        for(const auto& [type,name]:reversed)
            result.insert({name,type});
        return result;
    }(names_by_types);

std::flat_map<API_T,API> options_by_names=
{
    {API_T::COMMON,API(
            API::name_by_type(API_T::COMMON),
            1,
            API_T::COMMON)},
    #ifdef GRIB1API
    {API_T::GRIB1,API(
            API::name_by_type(API_T::GRIB1),
            1,
            API_T::GRIB1)}
    #endif
    #ifdef GRIB2API
    #ifdef GRIB1API
        ,
    #endif
    {API_T::GRIB2,API(
            API::name_by_type(API_T::GRIB2),
            2,
            API_T::GRIB1)}
    #endif
    #ifdef GRIB3API
    {API_T::GRIB3,3,"grib v3"}
    #endif
    #ifdef NETCDFAPI
    {API_T::NETCDF,0,"netcdf"}
    #endif
};

const std::vector<std::string_view>& API::accessible() noexcept{
    return types_by_names.keys();
}
std::optional<API_T> API::type_by_name(std::string_view api_name) noexcept{
    if(auto found = types_by_names.find(api_name);
            found!=types_by_names.end())
        return found->second;
    else return std::nullopt;
}
std::string_view API::name_by_type(API_T type) noexcept{
    if(auto found = names_by_types.find(type);
            found!=names_by_types.end())
        return found->second;
    else return {};
}
const API& API::type_options(API_T type) noexcept{
    return options_by_names.at(type);
}