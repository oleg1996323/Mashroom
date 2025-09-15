#include "config/server.h"
#include "definitions.h"

template<>
boost::json::value to_json(const network::server::Config& val){
    using namespace boost;
    json::object map;
    map["name"] = val.name_;
    map["host"] = val.settings_.host;
    map["service"] = val.settings_.service;
    map["port"] = val.settings_.port;
    map["protocol"] = val.settings_.protocol;
    map["timeout"] = val.settings_.timeout_seconds_;
    return map;
}

template<>
std::expected<network::server::Config,std::exception> from_json(const boost::json::value& val){
    network::server::Config result;
    if(val.is_object()){
        auto& c = val.as_object();
        try{
            result.name_=c.at("name").as_string();
            result.settings_.host=c.at("host").as_string();
            result.settings_.service=c.at("service").as_string();
            result.settings_.port=static_cast<network::Port>(c.at("port").as_uint64());
            result.settings_.protocol=static_cast<network::Protocol>(c.at("protocol").as_uint64());
            result.settings_.timeout_seconds_=c.at("timeout").as_int64();
            for(auto& accaddr:c.at("accaddr").as_array())
                result.accepted_addresses_.insert(accaddr.as_string().data());
        }
        catch(const boost::container::out_of_range& err){
            return std::unexpected(err);
        }
    }
    return result;
}