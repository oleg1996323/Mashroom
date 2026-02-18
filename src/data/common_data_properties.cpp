#include "data/common_data_properties.h"
#include "API/grib1/include/properties.h"

template<>
boost::json::value to_json(const CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props){
    boost::json::object result;
    result["center"]=to_json(props.center_);
    // result["fcst unit"]=to_json(props.fcst_unit_);
    result["table version"]=to_json(props.table_version_);
    result["indicator"]=to_json(props.parameter_);
    if(props.center_.has_value() && props.table_version_.has_value() && props.parameter_.has_value())
        if(auto* param = parameter_table(*props.center_,*props.table_version_,*props.parameter_);param)
            result["parameter name"]=to_json(std::string_view(param->name));
        else throw std::runtime_error("Does not contains parameter with presented table verson and parameter's number");
    return result;
}

template<>
std::expected<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json(const boost::json::value& value){
    CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
    if(value.is_object()){
        auto& val_obj = value.as_object();
        if(val_obj.contains("center")){
            if(auto center_res = from_json<Organization>(val_obj.at("center"));center_res.has_value())
                result.center_=center_res.value();
            else return std::unexpected(std::exception());
        }
        else return std::unexpected(std::exception());
        if(val_obj.contains("table version")){
            if(auto table_version_res = from_json<TimeFrame>(val_obj.at("table version"));table_version_res.has_value()){
                result.table_version_=table_version_res.value();
            }
            else return std::unexpected(std::exception());
        }
        else return std::unexpected(std::exception());
        if(val_obj.contains("indicator")){
            if(auto parameter_res = from_json<TimeFrame>(val_obj.at("indicator"));parameter_res.has_value()){
                result.parameter_=parameter_res.value();
            }
            else return std::unexpected(std::exception());
        }
        else return std::unexpected(std::exception());
        if(val_obj.contains("parameter name")){
            if(auto name_res = from_json<std::string>(val_obj.at("parameter name"));name_res.has_value()){
                if(auto* param = parameter_table(*result.center_,*result.table_version_,*result.parameter_); !param || param->name != name_res.value())
                    return std::unexpected(std::exception());
                else return result;
            }
            else return std::unexpected(std::exception());
        }
        else return std::unexpected(std::exception());
    }
    else return std::unexpected(std::exception());
}