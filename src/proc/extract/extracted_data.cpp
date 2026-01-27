#include "proc/extract/extracted_data.h"
#include "proc/extract/extracted_value.h"

template<>
boost::json::value to_json(const procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& data){
    boost::json::object result;
    result["common"] = to_json(data.cmn_);
    result["additional"] = to_json(data.add_);
    return result;
}

template<>
std::expected<procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json(const boost::json::value& value){
    procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
    if(value.is_object()){
        auto& val_obj = value.as_object();
        if(val_obj.contains("common")){
            if(auto common_res = from_json<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(val_obj.at("common"));common_res.has_value())
                result.cmn_=common_res.value();
            else return std::unexpected(std::exception());
        }
        else return std::unexpected(std::exception());
        if(val_obj.contains("additional")){
            if(auto additional_res = from_json<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(val_obj.at("additional"));additional_res.has_value()){
                result.add_=additional_res.value();
            }
            else return std::unexpected(std::exception());
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

template<>
boost::json::value to_json(const procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& add){
    boost::json::object result;
    result["forecast data"] = to_json(add.fcst_);
    result["level"]=to_json(add.level_);
    return result;
}

template<>
std::expected<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> 
    from_json<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& val){
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
    if(val.is_object()){
        auto& obj = val.as_object();
        if(obj.contains("forecast data") && obj.contains("level")){
            type result;
            if(auto fcst_res = from_json<TimeForecast>(obj.at("forecast data"));fcst_res.has_value())
                result.fcst_ = fcst_res.value();
            else return std::unexpected(std::exception());
            if(auto lvl_res = from_json<Level>(obj.at("level"));lvl_res.has_value())
                result.level_ = lvl_res.value();
            else return std::unexpected(std::exception());
        }
    }
    else
        return std::unexpected(std::exception());
}