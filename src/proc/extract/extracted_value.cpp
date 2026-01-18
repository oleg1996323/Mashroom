#include "proc/extract/extracted_value.h"

#include "types/time_interval.h"

template<>
boost::json::value to_json(const ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val){
    boost::json::object result;
    result["time"] = to_json<std::chrono::seconds>(val.time_date);
    result["value"] = to_json(val.value);
    return result;
}

template<>
std::expected<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& vals){
    if(vals.is_object()){
        ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
        auto& vals_obj = vals.as_object();
        if(vals_obj.contains("time") && vals_obj.contains("value")){
            if(auto time_res = from_json<std::decay_t<decltype(result.time_date)>>(vals_obj.at("time"));!time_res.has_value())
                return std::unexpected(std::exception());
            else result.time_date = *time_res;
            if(auto value_res = from_json<std::decay_t<decltype(result.value)>>(vals_obj.at("value"));!value_res.has_value())
                return std::unexpected(std::exception());
            else result.value = *value_res;
            return result;
        }
        else return std::unexpected(std::exception());
    }
    else return std::unexpected(std::exception());
}