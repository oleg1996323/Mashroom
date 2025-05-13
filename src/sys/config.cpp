#include "sys/config.h"
#include "cmd_parse/functions.h"
#include <cassert>
#include <network/common/def.h>
#include <boost/json.hpp>

namespace fs = std::filesystem;

namespace server{
    ServerConfig get_default_server_config(){
        ServerConfig config_;
        config_.settings_.host="10.10.10.10";
        config_.settings_.service="32396";
        config_.settings_.protocol="tcp";
        config_.settings_.timeout_seconds_ = 20;
        config_.name_ = "default";
        return config_;
    }
    ServerConfig::operator bool() const{
        return !settings_.host.empty() && (!settings_.service.empty() || !settings_.port.empty()) && !settings_.protocol.empty() && settings_.timeout_seconds_>=min_timeout_seconds && !name_.empty();
    }
    void ServerConfig::print_server_config(std::ostream& stream) const{
        stream<<"Server config name: \""<<name_<<"\""<<std::endl;
        stream<<"Host: \'"<<settings_.host<<"\' service: \'"<<settings_.service<<"\' port: \'"<<settings_.port<<"\' protocol: \'"<<settings_.protocol<<"\' timeout: "<<settings_.timeout_seconds_<<" seconds"<<std::endl;
        stream<<"Accepted addresses: ";
        auto joined = accepted_addresses_ | std::views::join_with(';');
        for (char addr : joined)
            stream << addr;
        stream<<std::endl;
    }
}
#include <boost/json/parser.hpp>
void Config::read(){
    using namespace boost;
    if(!fs::exists(config_mashroom_dir))
        if(!fs::create_directories(config_mashroom_dir))
            ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,config_mashroom_dir.c_str());
    if(fs::exists(config_mashroom_dir/uc_filename)){
        config_file.open(config_mashroom_dir/uc_filename,std::ios::in);
        if(!config_file.is_open())
            return;
        {
            json::stream_parser parser;
            json::error_code err_code;
            std::array<char,1024*4> buffer;
            while(config_file.good()){
                config_file.read(buffer.data(),buffer.size());
                parser.write(buffer.data(),config_file.gcount(),err_code);
                if(err_code)
                    ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,""s+(config_mashroom_dir/uc_filename).c_str()+" reading error",AT_ERROR_ACTION::ABORT);
            }
            if(!parser.done())
                return;
            else
                parser.finish();
            json::value root = parser.release();
            if(root.is_object()){
                for(const auto& [name,commands]:root.as_object()){
                    auto& commands_conf = configs_[name];
                    if(commands.is_array()){
                        commands_conf.reserve(commands.as_array().size());
                        for(const auto& command:commands.as_array())
                            commands_conf.push_back(command.as_string().data());
                    }
                }
            }
        }
        config_file.close();
    }
    if(fs::exists(config_mashroom_dir/sc_filename)){
        server_file.open(config_mashroom_dir/sc_filename,std::ios::in);
        if(!server_file.is_open())
            return;
        {
            json::stream_parser parser;
            json::error_code err_code;
            std::array<char,1024*4> buffer;
            while(server_file.good()){
                server_file.read(buffer.data(),buffer.size());
                parser.write(buffer.data(),server_file.gcount(),err_code);
                if(err_code)
                    ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,""s+(config_mashroom_dir/sc_filename).c_str()+" reading error",AT_ERROR_ACTION::ABORT);
            }
            if(!parser.done())
                return;
            else
                parser.finish();
            json::value root = parser.release();
            std::string last_config;
            if(root.is_object()){
                auto& name_configs = root.as_object();
                if(name_configs.contains("last"))
                    last_config=name_configs.at("last").as_string();
                if(name_configs.contains("configs") && name_configs.at("configs").is_object())
                    for(const auto& [name,config]:name_configs.at("configs").as_object()){
                        server::ServerConfig config_tmp;
                        config_tmp.name_=name;
                        auto& c = config.as_object();
                        if(c.contains("host"))
                            config_tmp.settings_.host=c.at("host").as_string();
                        if(c.contains("service"))
                            config_tmp.settings_.service=c.at("service").as_string();
                        if(c.contains("port"))
                            config_tmp.settings_.port=c.at("port").as_string();
                        if(c.contains("protocol"))
                            config_tmp.settings_.protocol=c.at("protocol").as_string();
                        if(c.contains("timeout"))
                            config_tmp.settings_.timeout_seconds_=c.at("timeout").as_int64();
                        if(c.contains("accaddr"))
                            for(auto& accaddr:c.at("accaddr").as_array())
                                config_tmp.accepted_addresses_.insert(accaddr.as_string().data());
                        server_configs_.insert(std::move(config_tmp));
                    }
                if(!last_config.empty())
                    server_config_ = get_server_config(last_config);
            }
        }
        server_file.close();
    }
    if(!server_config_)
        server_config_ = server::get_default_server_config();
    if(server_configs_.empty())
        server_configs_.insert(server_config_);
}
void Config::save(){
    write();
}
void Config::write(){
    using namespace boost;
    if(!config_file.is_open()){
        config_file.open(config_mashroom_dir/uc_filename,std::ios::out|std::ios::trunc);
        if(!config_file.is_open())
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Error at writing to the user-config file.",AT_ERROR_ACTION::ABORT,(config_mashroom_dir/uc_filename).c_str());
    }
    {
        json::value val;
        auto& obj = val.emplace_object();
        for(auto& [name,commands]:configs_){
            json::array arr;
            for(const std::string& command:commands)
                arr.emplace_back(command);
            obj[name]=arr;
        }
        config_file<<val.as_object();
        config_file.flush();
        std::cout<<"Saved user configurations to "<<config_mashroom_dir/uc_filename<<std::endl;
        config_file.close();
    }
    
    if(!server_file.is_open()){
        server_file.open(config_mashroom_dir/sc_filename,std::ios::out|std::ios::trunc);
        if(!server_file.is_open())
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Error at writing to the server-config file.",AT_ERROR_ACTION::ABORT,(config_mashroom_dir/sc_filename).c_str());
    }
    {
        json::value val;
        auto& last_configs = val.emplace_object();
        last_configs["last"]=server_config_.name_;
        auto& obj = last_configs["configs"].emplace_object();
        for(auto& s_conf:server_configs_){
            auto& name_conf = obj[s_conf.name_].emplace_object();
            name_conf["host"]=s_conf.settings_.host;
            name_conf["service"]=s_conf.settings_.service;
            name_conf["port"]=s_conf.settings_.port;
            name_conf["protocol"]=s_conf.settings_.protocol;
            name_conf["timeout"]=s_conf.settings_.timeout_seconds_;
            auto& accaddr = name_conf["accaddr"].emplace_array();
        for(const std::string& addr:s_conf.accepted_addresses_)
            accaddr.emplace_back(addr);
        }
        server_file<<val.as_object();
        server_file.flush();
        std::cout<<"Saved server configurations to "<<config_mashroom_dir/sc_filename<<std::endl;
        server_file.close();
    }
    
}
bool Config::add_user_config(const std::string& name,const std::vector<std::string>& commands){
    if(!configs_.contains(name)){
        configs_[name] = commands;
        save();
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::ALREADY_EXISTING_CONFIG_NAME,"",AT_ERROR_ACTION::CONTINUE,name);
    return false;
}
bool Config::add_user_config(std::string_view name,const std::vector<std::string_view>& commands){
    if(!configs_.contains(name.data())){
        configs_[name.data()] = std::vector<std::string>({commands.begin(),commands.end()});
        save();
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::ALREADY_EXISTING_CONFIG_NAME,"",AT_ERROR_ACTION::CONTINUE,name);
    return false;
}
bool Config::add_server_config(server::ServerConfig&& set){
    if(!server_configs_.contains(set)){
        server_configs_.insert(std::move(set));
        return true;
    }
    return false;
}
bool Config::remove_server_config(std::string_view name){
    if(name=="default")
        return false;
    else if(!server_configs_.contains(name))
        return false;
    else {
        server_configs_.erase(server_configs_.find(name));
        server_config_ = *server_configs_.find("default"sv);
        return true;
    }
}
bool Config::remove_server_config(const std::string& name){
    return remove_server_config(std::string_view(name));
}
bool Config::remove_user_config(const std::string& name){
    if(has_config_name(name)){
        configs_.erase(name);
        return true;
    }
    return false;
}
bool Config::remove_user_config(std::string_view name){
    if(has_config_name(name)){
        configs_.erase(name.data());
        return true;
    }
    return false;
}
bool change_user_config(std::string_view name,const std::vector<std::string_view>& commands){
    assert(false);
}
bool change_user_config(const std::string& name,const std::vector<std::string>& commands){
    assert(false);
}
const std::unordered_map<std::string,std::vector<std::string>> Config::get_user_configs() const{
    return configs_;
}
const std::vector<std::string>& Config::get_user_config(const std::string& name) const{
    if(has_config_name(name))
        return configs_.at(name);
    else return empty_config_;
}
const std::vector<std::string>& Config::get_user_config(std::string_view name) const{
    if(has_config_name(name))
        return configs_.at(name.data());
    else return empty_config_;
}
const server::ServerConfig& Config::get_server_config(std::string_view name) const{
    if(has_server_config(name))
        return *server_configs_.find(name);
    else return empty_server_config_;
}
const server::ServerConfig& Config::get_server_config(const std::string& name) const{
    return get_server_config(std::string_view(name));
}
const server::ServerConfig& Config::get_current_server_config() const{
    return server_config_;
}
bool Config::set_current_server_config(std::string_view name) const{
    if(server_configs_.contains(name)){
        server_config_ = *server_configs_.find(name);
        return true;
    }
    return false;
}
bool Config::set_current_server_config(const std::string& name) const{
    return set_current_server_config(std::string_view(name));
}
bool Config::set_current_server_config(server::ServerConfig&& config){
    if(config){
        server_configs_.insert(std::move(config));
        return true;
    }
    return false;
}
const server::ServerConfig& Config::current_server_setting(){
    return server_config_;
}
//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)