#include "data/msg.h"

using namespace boost;
using namespace std::string_view_literals;

template<>
std::expected<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json(const boost::json::value& val){
    if(auto obj = val.if_object();obj){
        FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
        if(obj->contains("position") && obj->at("position").is_uint64())
            result.buf_pos_=obj->at("position").as_int64();
        if(obj->contains("message size") && obj->at("message size").is_uint64())
            result.msg_sz_=obj->at("message size").as_int64();
        if(obj->contains("center") && obj->at("center").is_uint64())
            result.center=static_cast<Organization>(obj->at("center").as_int64());
        if(obj->contains("table version") && obj->at("table version").is_uint64())
            result.table_version=obj->at("table version").as_int64();
        if(obj->contains("parameter") && obj->at("parameter").is_uint64())
            result.parameter=obj->at("parameter").as_int64();
        if(obj->contains("time") && obj->at("time").is_string()){
            auto time = obj->at("time").as_string();
            std::istringstream stream(time.c_str());
            std::chrono::from_stream(stream,"%Y/%m/%d %h:%M:%S GMT",result.date);
        }
        if(obj->contains("forecast data") && obj->at("forecast data").is_uint64())
            if(auto fcst_data = from_json<TimeForecast>(obj->at("forecast data"));fcst_data.has_value())
            result.t_unit = fcst_data.value();
        else return std::unexpected(fcst_data.error());
        if(obj->contains("level") && obj->at("level").is_object()){
            if(auto level_tmp = from_json<Level>(obj->at("level").as_object());level_tmp.has_value())
                result.level_= level_tmp.value();            
        }
        if(obj->contains("grid"))
            if(auto grid_result = from_json<decltype(result.grid_data)>(obj->at("grid"));!grid_result.has_value())
                result.grid_data = grid_result.value();
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val){
    boost::json::object obj;
    
    obj["position"]=val.buf_pos_;
    obj["message size"]=val.msg_sz_;
    obj["center"]=center_to_abbr(val.center);
    obj["table version"]=val.table_version;
    obj["parameter"]=val.parameter;
    obj["time"]=std::format("{:%Y/%m/%d %h:%M:%S} GMT",time_point_cast<std::chrono::seconds>(val.date));
    obj["forecast data"]=to_json(val.t_unit);
    obj["grid"]=to_json(val.grid_data);
    return obj;
}

// template<>
// std::expected<FileMsgVariant,std::exception> from_json<FileMsgVariant>(const boost::json::value& val){
//     if(!val.is_object())
//         return std::unexpected(std::exception());
//     else{
//         auto& obj = val.as_object();
//         if(!obj.contains("type") || !obj.contains("format") || 
//             !obj.at("type").is_string() || !obj.at("format").is_string())
//             return std::unexpected(std::exception());
//         else{
//             if(obj.at("type").as_string()=="undefined" || obj.at("format").as_string()=="undefined")
//                 return std::monostate();
//             else if(obj.at("type").as_string()=="time series" && obj.at("format").as_string()=="grib v1")
//                 if(!obj.contains("data"))
//                     return std::unexpected(std::exception());
//                 else return from_json<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(obj.at("data"));
//             else return std::unexpected(std::exception());
//         }
//     }
// }

// template<>
// boost::json::value to_json(const FileMsgVariant& val){
//     boost::json::object obj;

//     auto lambda = [&obj](const auto& value){
//         using type = std::decay_t<decltype(value)>;
//         if constexpr(std::is_same_v<type,std::monostate>){
//             obj["type"]="undefined";
//             obj["format"]="undefined";
//         }
//         else if constexpr(std::is_same_v<type,FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>){
//             obj["type"]="time series";
//             obj["format"]="grib v1";
//             obj["data"] = to_json(value);
//         }
//         else static_assert(false,"Not implemeted");
//     };
//     std::visit(lambda,val);
//     return obj;
// }