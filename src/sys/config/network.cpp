#include "config/network.h"

template<>
boost::json::value to_json(const network::Config& val){
    boost::json::object result;
    result["client"] = to_json(val.client_config());
    result["server"] = to_json(val.server_config());
    return result;
}

template<>
std::expected<network::Config,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        auto& obj = val.as_object();
        network::Config result;
        if(obj.contains("client")){
            if(auto client_res = from_json<network::client::Config>(obj.at("client"));
                client_res.has_value())
                result.client_config(std::move(client_res.value()));
            else return std::unexpected(client_res.error());
        }
        if(obj.contains("server")){
            if(auto server_res = from_json<network::server::Config>(obj.at("server"));
                server_res.has_value())
                result.server_config(std::move(server_res.value()));
            else return std::unexpected(server_res.error());
        }
        return result;
    }
    else if(val.is_null())
        return network::Config();
    else return std::unexpected(std::runtime_error("not object-type"));
}