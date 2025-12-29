#include "proc/extract/extracted_value.h"

template<>
boost::json::value to_json(const ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>& data){
    boost::json::array result;
    int count = 0;
    for(auto& [common,values]:data){
        if(values.empty())
            continue;
        result.emplace_back(boost::json::object());
        auto& data_series = result.back().as_object();
        data_series["info"]=to_json(common);
        data_series["values"] = boost::json::array();
        auto& data = data_series["values"].as_array();
        for(auto& value:values)
            data.push_back(to_json(value));
    }
    return result;
}

template<>
std::expected<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& data){
    ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
    if(data.is_array()){
        for(auto& vals:data.as_array()){
            if(vals.is_object()){
                auto& vals_obj = vals.as_object();
                if(vals_obj.contains("info") && vals_obj.contains("values")){
                    if(auto info_res = from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>::key_type>(vals_obj.at("info"));info_res.has_value()){
                        if(auto values_res = from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>::mapped_type>(vals_obj.at("values"));values_res.has_value())
                            result[info_res.value()] = std::move(*values_res);
                        else return std::unexpected(std::exception());
                    }
                    else return std::unexpected(std::exception());
                }
                else return std::unexpected(std::exception());
            }
            else if(vals.is_null())
                continue;
            else return std::unexpected(std::exception());
        }
        return result;
    }
    else return std::unexpected(std::exception());
}

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

template<>
boost::json::value to_json(const ExtractedData& vals){
    auto conv_lambda_visitor = [](const auto& val){
        boost::json::value result;
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>,std::monostate>)
            return result;
        else{
            result = to_json(val);
            return result;
        }
    };
    return std::visit(conv_lambda_visitor,vals);
}