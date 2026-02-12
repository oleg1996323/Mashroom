#include "proc/index/write/json.h"

template<>
std::expected<std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>,std::exception> from_json(const boost::json::value& val){
    std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> result;
    if(auto arr = val.if_array();arr){
        for(auto& msg:*arr)
            if(std::expected<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>, std::exception> res_msg = from_json<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(msg);res_msg.has_value())
                result.emplace_back(res_msg.value());
            else return std::unexpected(std::invalid_argument("invalid JSON struct"));
        return result;
    }
    else
        return std::unexpected(std::invalid_argument("invalid JSON struct"));
}

template<>
boost::json::value to_json(const std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& val);