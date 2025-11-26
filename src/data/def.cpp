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

std::string_view preferred_extension(Data_f format){
    return token_to_extensions(format).front();
}

std::optional<std::vector<Data_f>> extension_to_tokens(std::string_view extension) noexcept{
    if(auto found = extension_name.find(extension);found!=extension_name.end())
        return found->second;
    else return std::nullopt;
}

const std::vector<std::string_view>& token_to_extensions(__Data__::FORMAT type_extension) noexcept{
    return extension_token.at(type_extension);
}

std::optional<__Data__::FORMAT> text_to_data_format(std::string_view text) noexcept{
    auto str = boost::algorithm::to_lower_copy(text);
    if(auto found = data_format_name.find(str);found!=data_format_name.end())
        return found->second;
    else return std::nullopt;
}
using namespace std::string_literals;
std::string filename_by_type(__Data__::FORMAT format){
    return ""s+bindata_filename.data()+preferred_extension(format).data();
}