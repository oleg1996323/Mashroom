#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "sys/error_print.h"
#include "sys/error_code.h"

#include <type_traits>

namespace fs = std::filesystem;
namespace network::server{
struct Settings{
    std::string host;
    std::string service;
    std::string port;
    std::string protocol;
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
    bool operator()(std::string_view lhs,network::server::Config&& rhs) const{
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
#include <filesystem>
namespace fs = std::filesystem;

static fs::path get_config_dir(){
    fs::path res = fs::path(getenv("HOME"));
    res/=CONFIG_MASHROOM_DIR_REL;
    return res;
}

constexpr const char* uc_filename = "usr.txt";
constexpr const char* sc_filename = "server.txt";

class Config{
    mutable network::server::Config server_config_;
    std::unordered_map<std::string,std::vector<std::string>> configs_;
    std::unordered_set<network::server::Config> server_configs_;
    const fs::path config_mashroom_dir = get_config_dir();
    inline static const std::vector<std::string> empty_config_{};
    inline static const network::server::Config empty_server_config_{};
    std::fstream config_file;
    std::fstream server_file;

    public:
    Config(){
        read();
    }
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    ~Config(){
        save();
        config_file.close();
    }
    bool add_user_config(std::string_view name,const std::vector<std::string_view>& commands);
    bool add_user_config(const std::string& name,const std::vector<std::string>& commands);
    bool add_server_config(network::server::Config&&);
    bool change_user_config(std::string_view name,const std::vector<std::string_view>& commands);
    bool change_user_config(const std::string& name,const std::vector<std::string>& commands);
    bool remove_user_config(const std::string& name);
    bool remove_user_config(std::string_view name);
    bool remove_server_config(std::string_view name);
    bool remove_server_config(const std::string& name);
    void save();
    bool has_config_name(const std::string& name) const{
        return configs_.contains(name);
    }
    bool has_config_name(std::string_view name) const{
        return configs_.contains(name.data());
    }
    bool has_server_config(std::string_view name) const{
        return server_configs_.contains(name);
    }
    bool has_server_config(const std::string& name) const{
        return server_configs_.contains(name);
    }
    bool set_current_server_config(std::string_view name) const;
    bool set_current_server_config(const std::string& name) const;
    bool set_current_server_config(network::server::Config&& config);
    const std::unordered_map<std::string,std::vector<std::string>> get_user_configs() const;
    const std::vector<std::string>& get_user_config(const std::string& name) const;
    const std::vector<std::string>& get_user_config(std::string_view name) const;
    const network::server::Config& get_server_config(std::string_view name) const;
    const network::server::Config& get_server_config(const std::string& name) const;
    const network::server::Config& get_current_server_config() const;
    const network::server::Config& current_server_setting();
    private:
    void read();
    void write();
};

//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)