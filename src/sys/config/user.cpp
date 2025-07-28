#include "config/user.h"

template<>
boost::json::value to_json(const user::Settings& val){
    using namespace boost;
    json::object map;
    map["name"] = val.name_;
    map["mashroom-update-ti"] = to_json(val.mashroom_update_ti_);
    map["capitalize-update-ti"] = to_json(val.capitalize_update_ti_);
    return map;
}

template<>
std::expected<user::Settings,std::exception> from_json(const boost::json::value& val){
    user::Settings result;
    try{
        result.name_ = val.at("name").as_string();
        {
            std::expected<TimePeriod, std::exception> json_res = from_json<TimePeriod>(
                val.at("mashroom-update-ti"));
            if(json_res.has_value())
                result.mashroom_update_ti_ = std::move(json_res.value());
            else return std::unexpected(json_res.error());
        }
        {
            std::expected<TimePeriod, std::exception> json_res = from_json<TimePeriod>(
                val.at("capitalize-update-ti"));
            if(json_res.has_value())
                result.capitalize_update_ti_ = std::move(json_res.value());
            else return std::unexpected(json_res.error());
        }
    }
    catch(const std::exception& err){
        return std::unexpected(err);
    }
    return result;
}