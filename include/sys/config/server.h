#pragma once
#include <string>
#include <unordered_set>
#include "boost_functional/json.h"
#include "serversettings.h"

namespace network::server{

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