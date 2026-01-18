#include "data/def.h"
#include <string.h>
#include <boost/algorithm/string.hpp>

const std::unordered_map<Data_f,std::vector<std::string_view>> extension_token = {
                    {Data_f::GRIB_v1,{std::string_view(".grib1"),std::string_view(".grib"),std::string_view(".gb"),std::string_view(".grb")}}
};
                    /* {"hgt",Data_f::HGT},
                    {"netCDF",Data_f::NETCDF} };*/

const std::unordered_map<std::string_view,std::vector<Data_f>> extension_name = [](const std::unordered_map<Data_f,std::vector<std::string_view>>& tokens){
    std::unordered_map<std::string_view,std::vector<Data_f>> result;
    for(auto& [token,txts]:tokens)
        for(auto txt:txts)
            result[txt].push_back(token);
    return result;
}(extension_token);

const std::unordered_map<Data_f,std::string_view> utility_extension_token = {
                    {Data_f::GRIB_v1,std::string_view(".g1bd")}
};

const std::unordered_map<std::string_view,Data_f> utility_extension_name = [](const std::unordered_map<Data_f,std::string_view>& tokens){
    std::unordered_map<std::string_view,Data_f> result;
    for(auto& [token,txt]:tokens)
        result[txt]=token;
    return result;
}(utility_extension_token);

const std::unordered_map<Data_f,std::string_view> data_format_token = {
                    {Data_f::GRIB_v1,{std::string_view("Grib1")}}
};
                    /* {"hgt",Data_f::HGT},
                    {"netCDF",Data_f::NETCDF} };*/

const std::unordered_map<std::string_view,Data_f> data_format_name = [](const std::unordered_map<Data_f,std::string_view>& tokens){
    std::unordered_map<std::string_view,Data_f> result;
    for(auto& [token,txt]:tokens)
        result[txt]=token;
    return result;
}(data_format_token);

const std::unordered_map<Data_t,std::string_view> data_type_token = {
                    {Data_t::TIME_SERIES,{std::string_view("meteo")}},
                    {Data_t::POLYGONE,{std::string_view("polygone")}},
                    {Data_t::GRID,{std::string_view("grid")}},
                    {Data_t::ISOLINES,{std::string_view("isolines")}}
};
                    /* {"hgt",Data_f::HGT},
                    {"netCDF",Data_f::NETCDF} };*/

const std::unordered_map<std::string_view,Data_t> data_type_name = [](const std::unordered_map<Data_t,std::string_view>& tokens){
    std::unordered_map<std::string_view,Data_t> result;
    for(auto& [token,txt]:tokens)
        result[txt]=token;
    return result;
}(data_type_token);

std::size_t number_of_formats() noexcept{
    return data_format_token.size();
}
std::size_t number_of_types() noexcept{
    return data_type_token.size();
}

std::string_view preferred_extension(Data_f format){
    return token_to_extensions(format).front();
}

std::optional<std::vector<Data_f>> extension_to_tokens(std::string_view extension) noexcept{
    if(auto found = extension_name.find(extension);found!=extension_name.end())
        return found->second;
    else return std::nullopt;
}

const std::vector<std::string_view>& token_to_extensions(Data_f token) noexcept{
    return extension_token.at(token);
}

std::string_view to_data_format_name(Data_f token) noexcept{
    return data_format_token.at(token);
}
using namespace std::string_literals;
std::string filename_by_format(Data_f format){
    return ""s+bindata_filename.data()+preferred_extension(format).data();
}

std::optional<Data_f> to_data_format_token(std::string_view text) noexcept{
    if(auto found = data_format_name.find(text);found!=data_format_name.end())
        return found->second;
    else return std::nullopt;
}
std::optional<Data_t> to_data_type_token(std::string_view text) noexcept{
    if(auto found = data_type_name.find(text);found!=data_type_name.end())
        return found->second;
    else return std::nullopt;
}
std::string_view to_data_type_name(Data_t token) noexcept{
    return data_type_token.at(token);
}

std::string_view utility_extension(Data_f format) noexcept{
    return utility_extension_token.at(format);
}
std::optional<Data_f> utility_token(std::string_view extension) noexcept{
    if(auto found = utility_extension_name.find(extension);found!=utility_extension_name.end())
        return found->second;
    else return std::nullopt;
}