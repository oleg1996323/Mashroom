#pragma once
#include <string>
#include <unordered_set>
#include "boost_functional/json.h"

namespace network::server{
struct Settings{
    std::string host;
    std::string service;
    std::string port;
    std::string protocol = "tcp";
    int timeout_seconds_=20;
    Settings() = default;
    Settings(const Settings& other)
    {
        if(this!=&other){
            host=other.host;
            service=other.service;
            port=other.port;
            protocol=other.protocol;
            timeout_seconds_=other.timeout_seconds_;
        }
    }
    Settings(Settings&& other)
    {   
        if(this!=&other){
            host=std::move(other.host);
            service=std::move(other.service);
            port=std::move(other.port);
            protocol=std::move(other.protocol);
            timeout_seconds_=other.timeout_seconds_;
        }
    }
    Settings& operator=(const Settings& other){
        if(this!=&other){
            host=other.host;
            service=other.service;
            port=other.port;
            protocol=other.protocol;
            timeout_seconds_=other.timeout_seconds_;
        }
        return *this;
    }
    Settings& operator=(Settings&& other){
        if(this!=&other){
            host.swap(other.host);
            service.swap(other.service);
            port.swap(other.port);
            protocol.swap(other.protocol);
            timeout_seconds_=other.timeout_seconds_;
        }
        return *this;
    }  
};

struct Config{
    network::server::Settings settings_;
    std::unordered_set<std::string> accepted_addresses_;
    std::string name_;
    Config() = default;
    operator bool() const;
    void print_server_config(std::ostream&) const;
};

Config get_default_server_config();
}
template<>
struct std::hash<network::server::Config>{
    using is_transparent = std::true_type;
    size_t operator()(const network::server::Config& config) const{
        return std::hash<std::string>{}(config.name_);
    }
    size_t operator()(network::server::Config&& config) const{
        return std::hash<std::string>{}(config.name_);
    }
    size_t operator()(const std::string& name) const{
        return std::hash<std::string>{}(name);
    }
    size_t operator()(std::string_view name) const{
        return std::hash<std::string_view>{}(name);
    }
};

template<>
struct std::equal_to<network::server::Config>{
    using is_transparent = std::true_type;
    bool operator()(const network::server::Config& lhs,const network::server::Config& rhs) const{
        return lhs.name_==rhs.name_;
    }
    bool operator()(const std::string& lhs,const network::server::Config& rhs) const{
        return lhs==rhs.name_;
    }
    bool operator()(const network::server::Config& lhs,const std::string& rhs) const{
        return lhs.name_==rhs;
    }
    bool operator()(const network::server::Config& lhs,std::string_view rhs) const{
        return lhs.name_==rhs;
    }
    bool operator()(std::string_view lhs,const network::server::Config& rhs) const{
        return lhs==rhs.name_;
    }
};

template<>
struct std::less<network::server::Config>{
    using is_transparent = std::true_type;
    bool operator()(const network::server::Config& lhs,const network::server::Config& rhs) const{
        return lhs.name_<rhs.name_;
    }
    bool operator()(const std::string& lhs,const network::server::Config& rhs) const{
        return lhs<rhs.name_;
    }
    bool operator()(const network::server::Config& lhs,const std::string& rhs) const{
        return lhs.name_<rhs;
    }
    bool operator()(std::string_view lhs,const network::server::Config& rhs) const{
        return lhs<rhs.name_;
    }
    bool operator()(const network::server::Config& lhs,std::string_view rhs) const{
        return lhs.name_<rhs;
    }
};

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const network::server::Config& val);

template<>
std::expected<network::server::Config,std::exception> from_json(const boost::json::value& val);