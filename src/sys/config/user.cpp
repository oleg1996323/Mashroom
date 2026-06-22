#include "config/user.h"


template<>
boost::json::value to_json(const user::Config& val){
    using namespace boost;
    json::object map;
    return val.base_to_json();
}

template<>
std::expected<user::Config,std::exception> 
        from_json(const boost::json::value& val){
    user::Config result;
    if(val.is_object()){
        auto& c = val.as_object();
        if(auto base_res = result.base_from_json(val);
            base_res.has_value())
            return std::unexpected(base_res.value());
    }
    else if(val.is_null())
        return result;
    else return std::unexpected(std::invalid_argument("not object-type in client-config"));
    return result;
}
template<>
boost::json::value to_json(const user::Settings& val){
    using namespace boost;
    json::object map;
    map["mashroom-update-ti"] = to_json(val.mashroom_update_ti);
    map["index-update-ti"] = to_json(val.index_update_ti);
    return map;
}

template<>
std::expected<user::Settings,std::exception> from_json(const boost::json::value& val){
    user::Settings result;
    {
        std::expected<DateTimeDiff, std::exception> json_res = from_json<DateTimeDiff>(
            val.at("mashroom-update-ti"));
        if(json_res.has_value())
            result.mashroom_update_ti = std::move(json_res.value());
        else return std::unexpected(json_res.error());
    }
    {
        std::expected<DateTimeDiff, std::exception> json_res = from_json<DateTimeDiff>(
            val.at("index-update-ti"));
        if(json_res.has_value())
            result.index_update_ti = std::move(json_res.value());
        else return std::unexpected(json_res.error());
    }
    return result;
}