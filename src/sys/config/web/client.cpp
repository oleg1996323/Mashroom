#include "config/web/client.h"

template<>
boost::json::value to_json(const network::client::Config& val){
    using namespace boost;
    json::object map;
    boost::json::array configurations;
    for(auto& [name,settings]:val.configurations()){
        json::object tmp;
        tmp["name"] = name;
        tmp["settings"] = to_json(settings);
        configurations.push_back(tmp);
    }
    map["configurations"] = std::move(configurations);
    return map;
}

template<>
std::expected<network::client::Config,std::exception> 
        from_json(const boost::json::value& val){
    network::client::Config result;
    if(val.is_object()){
        auto& c = val.as_object();
        if(c.contains("configurations")){
            if(c.at("configurations").is_array()){
                auto& arr = c.at("configurations").as_array();
                for(auto& arr_val:arr){
                    if(arr_val.is_object()){
                        auto& pair = arr_val.as_object();
                        if(pair.contains("name") && pair.contains("settings"))
                            if(auto name_tmp = from_json<decltype(result)::name_t>(pair.at("name"));
                                name_tmp.has_value())
                            {
                                if(name_tmp.value().empty())
                                    return std::unexpected(
                                        std::invalid_argument("\"name\" empty"));
                                if(auto sets_tmp = from_json<decltype(result)::settings_t>(pair.at("settings"));
                                    sets_tmp.has_value())
                                    result.add(name_tmp.value(),std::move(sets_tmp.value()));                                
                            }
                            else if(pair.at("name").is_null())
                                return std::unexpected(
                                    std::invalid_argument("\"name\" empty"));
                            else return std::unexpected(
                                    std::invalid_argument("\"name\" not string"));
                    }
                }
            }
        }
    }
    return result;
}