#include "data/datastruct/searchdataresult.h"

template<>
std::expected<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> 
    from_json<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& val)
{
    if(val.is_object()){
        auto& obj = val.as_object();
        if(!obj.contains("time sequence"))
            return std::unexpected(std::invalid_argument(
            "missing \"time sequence\" field at parsing \"additional\" json data of SearchDataResult type"));
        if(!obj.contains("level"))
            return std::unexpected(std::invalid_argument(
            "missing \"level\" field at parsing \"additional\" json data of SearchDataResult type"));
        if(!obj.contains("forecast"))
            return std::unexpected(std::invalid_argument(
            "missing \"forecast\" field at parsing \"additional\" json data of SearchDataResult type"));
        find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
        if(obj.contains("grid")){
            if(auto grid_parse = from_json<decltype(result.grid_)::element_type>(obj.at("grid"));
                    !grid_parse.has_value())
                return std::unexpected(grid_parse.error());
            else result.grid_ = std::make_shared<GridInfo>(
                    std::move(grid_parse.value()));
        }
        if(auto ts_parse = from_json<decltype(result.ts_)>(obj.at("time sequence"));
                !ts_parse.has_value())
            return std::unexpected(ts_parse.error());
        else result.ts_ = std::move(ts_parse.value());
        if(auto lvl_parse = from_json<decltype(result.lvl_)>(obj.at("level"));
                !lvl_parse.has_value())
            return std::unexpected(lvl_parse.error());
        else result.lvl_ = std::move(lvl_parse.value());
        if(auto fcst_parse = from_json<decltype(result.fcst_)>(obj.at("forecast"));
                !fcst_parse.has_value())
            return std::unexpected(fcst_parse.error());
        else result.fcst_ = std::move(fcst_parse.value());
        return result;
    }
    else if(val.is_null())
        return find_data_info::
            details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    else return std::unexpected(std::invalid_argument(
            "not object-type detected at parsing json data for SearchDataResult"));
}

template<>
boost::json::value to_json(const 
        find_data_info::details::Additional<
            Data_t::TIME_SERIES,Data_f::GRIB_v1>& val)
{
    boost::json::object result;
    if(val.grid_)
        result["grid"]=to_json(*val.grid_);
    result["time sequence"]=to_json(val.ts_);
    result["level"]=to_json(val.lvl_);
    result["forecast"]=to_json(val.fcst_);
    return result;
}