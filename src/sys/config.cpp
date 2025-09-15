#include "config.h"
#include <cassert>
#include <boost/json.hpp>
#include <ranges>
#include "filesystem.h"
#include "proj.h"
#include <netdb.h>

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;
namespace network::server{
    constexpr int min_timeout_seconds = 1;
    Config get_default_server_config(){
        Config config_;
        config_.settings_.host="10.10.10.10";
        config_.settings_.service="32396";
        config_.settings_.protocol=static_cast<network::Protocol>(getprotobyname("tcp")->p_proto);
        config_.settings_.timeout_seconds_ = 20;
        config_.name_ = "default";
        return config_;
    }
    Config::operator bool() const{
        return !settings_.host.empty() && (!settings_.service.empty() || !settings_.port>0) && !(settings_.protocol<0) && settings_.timeout_seconds_>=min_timeout_seconds && !name_.empty();
    }
    void Config::print_server_config(std::ostream& stream) const{
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
#include "boost_functional/json.h"
void Config::read_user_config() noexcept{
    using namespace boost;
    std::expected<json::value,std::error_code> root = parse_json(
        sys_settings_.config_dir/uc_filename);
    if(!root.has_value()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,root.error().message(),AT_ERROR_ACTION::CONTINUE);
        return;
    }
    std::string last = "";
    if(root.value().is_object()){
        auto& registered = root.value().as_object();
        if(registered.contains("last") && registered.at("last").is_string())
            last = registered.at("last").as_string();
        if(registered.contains("configs") && registered.at("configs").is_array())
            for(const auto& settings_val:registered.at("configs").as_array()){
                auto settings = from_json<user::Settings>(settings_val);
                if(settings.has_value())
                    configs_.insert(std::move(settings.value()));
                else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                            "configuration corrupted",AT_ERROR_ACTION::CONTINUE);
            }
        if(last.empty() || !configs_.contains(last)){
            user_config_ = *configs_.insert(user::default_config()).first;
        }
    }
}

void Config::read_server_config() noexcept{
    using namespace boost;
    std::expected<json::value,std::error_code> root = parse_json(
                    sys_settings_.server_config_dir/sc_filename);
    if(!root.has_value()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,root.error().message(),AT_ERROR_ACTION::CONTINUE);
        return;
    }
    std::string last_config;
    if(root.value().is_object()){
        auto& name_configs = root.value().as_object();
        if(name_configs.contains("last"))
            last_config=name_configs.at("last").as_string();
        if(name_configs.contains("configs") && name_configs.at("configs").is_object())
            for(const auto& config:name_configs.at("configs").as_array()){
                auto res = from_json<network::server::Config>(config);
                if(!res.has_value())
                    ErrorPrint::print_error(ErrorCode::DESERIALIZATION_ERROR,
                        "server configuration corrupted",AT_ERROR_ACTION::CONTINUE);
                else
                    server_configs_.insert(std::move(res.value()));
            }
        if(!last_config.empty())
            server_config_ = get_server_config(last_config);
    }
    if(!server_config_)
        server_config_ = network::server::get_default_server_config();
    if(server_configs_.empty())
        server_configs_.insert(server_config_);
}
void Config::save(){
    write_user_config();
    write_server_config();
}
void Config::write_user_config(){
    using namespace boost;
    if(!directory_accessible(sys_settings_.config_dir)){
        ErrorPrint::print_error(ErrorCode::CANNOT_ACCESS_PATH_X1,"",AT_ERROR_ACTION::CONTINUE,(sys_settings_.config_dir).string());
        return;
    }
    std::ofstream file(sys_settings_.config_dir/uc_filename,std::ios::out|std::ios::trunc);
    if(!file.is_open()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
            "Error at writing to the user-config file.",
            AT_ERROR_ACTION::CONTINUE,(sys_settings_.config_dir/uc_filename).c_str());
        return;
    }
    {
        json::value val;
        auto& configs = val.emplace_object();
        for(auto& settings:configs_){
            configs[settings.name_]=to_json(settings);
        }
        file<<val.as_object();
        file.flush();
        std::cout<<"Saved user configurations to "<<
        sys_settings_.config_dir/uc_filename<<std::endl;
        file.close();
    }
}
void Config::write_server_config(){
    using namespace boost;
    if(!directory_accessible(sys_settings_.server_config_dir)){
        ErrorPrint::print_error(ErrorCode::CANNOT_ACCESS_PATH_X1,"",AT_ERROR_ACTION::CONTINUE,(sys_settings_.server_config_dir).string());
        return;
    }
    std::ofstream file(sys_settings_.server_config_dir/sc_filename,std::ios::out|std::ios::trunc);
    if(!file.is_open()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
            "Error at writing to the server-config file.",
            AT_ERROR_ACTION::CONTINUE,(sys_settings_.server_config_dir/sc_filename).c_str());
        return;
    }
    {
        json::value val;
        auto& last_configs = val.emplace_object();
        last_configs["last"]=server_config_.name_;
        auto& configs = last_configs["configs"].emplace_array();
        for(auto& s_conf:server_configs_)
            configs.emplace_back(to_json(s_conf));
        file<<val.as_object();
        file.flush();
        std::cout<<"Saved server configurations to "<<sys_settings_.server_config_dir/sc_filename<<std::endl;
        file.close();
    }
    
}

ErrorCode Config::set_log_directory(const fs::path& path) noexcept{
    if(!fs::exists(path))
        if(fs::create_directories(path))
            return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
        else{
            sys_settings_.log_dir = path;
            return ErrorCode::NONE;
        }
    else {
        sys_settings_.log_dir = path;
        return ErrorCode::NONE;
    }
}
#include "filesystem.h"
ErrorCode Config::set_config_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        sys_settings_.config_dir = path;
        return ErrorCode::NONE;
    }
}
ErrorCode Config::set_server_config_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        sys_settings_.server_config_dir = path;
        return ErrorCode::NONE;
    }
}

bool Config::add_user_config(const user::Settings& settings) noexcept{
    if(settings.name_.empty()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"config settings name",AT_ERROR_ACTION::CONTINUE);
        return false;
    }
    if(!configs_.contains(settings.name_)){
        configs_.insert(settings);
        save();
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::ALREADY_EXISTING_CONFIG_NAME,"",AT_ERROR_ACTION::CONTINUE,settings.name_);
    return false;
}
bool Config::add_server_config(network::server::Config&& set) noexcept{
    if(!server_configs_.contains(set)){
        server_configs_.insert(std::move(set));
        return true;
    }
    return false;
}
bool Config::remove_server_config(std::string_view name) noexcept{
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
bool Config::remove_server_config(const std::string& name) noexcept{
    return remove_server_config(std::string_view(name));
}
bool Config::remove_user_config(const std::string& name) noexcept{
    return remove_user_config(std::string_view(name));
}
bool Config::remove_user_config(std::string_view name) noexcept{
    if(has_config_name(name)){
        auto found = configs_.find(name);
        configs_.erase(found);
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::CONFIG_NAME_DOESNT_EXISTS_X1,"",AT_ERROR_ACTION::CONTINUE,name);
    return false;
}
bool Config::setup_user_config(const user::Settings& settings) noexcept{
    if(has_config_name(settings.name_)){
        configs_.insert(settings);
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::CONFIG_NAME_DOESNT_EXISTS_X1,"",AT_ERROR_ACTION::CONTINUE,settings.name_);
    return false;
}
bool Config::setup_server_config(network::server::Config&& settings) noexcept{
    if(server_configs_.contains(settings) && settings){
        server_configs_.insert(std::move(settings));
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::CONFIG_NAME_DOESNT_EXISTS_X1,"",AT_ERROR_ACTION::CONTINUE,settings.name_);
    return false;
}
const std::unordered_set<user::Settings,user::SettingsHash,user::SettingsEqual>& Config::get_user_configs() const noexcept{
    return configs_;
}
const std::unordered_set<network::server::Config>& Config::get_server_configs() const noexcept{
    return server_configs_;
}
const user::Settings& Config::get_user_config(const std::string& name) const noexcept{
    return get_user_config(std::string_view(name));
}
const user::Settings& Config::get_user_config(std::string_view name) const noexcept{
    if(name.empty()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"configuration name",AT_ERROR_ACTION::CONTINUE);
        return empty_config_;
    }
    if(has_config_name(name)){
        auto found = configs_.find(name);
        return *found;
    }
    else{
        ErrorPrint::print_error(ErrorCode::CONFIG_NAME_DOESNT_EXISTS_X1,"",AT_ERROR_ACTION::CONTINUE,name);
        return empty_config_;
    }
}
const user::Settings& Config::get_current_user_config() const noexcept{
    return user_config_;
}
const network::server::Config& Config::get_server_config(std::string_view name) const noexcept{
    if(has_server_config(name))
        return *server_configs_.find(name);
    else return empty_server_config_;
}
const network::server::Config& Config::get_server_config(const std::string& name) const noexcept{
    return get_server_config(std::string_view(name));
}
const network::server::Config& Config::get_current_server_config() const noexcept{
    return server_config_;
}
bool Config::set_server_config(std::string_view name) const noexcept{
    if(server_configs_.contains(name)){
        server_config_ = *server_configs_.find(name);
        return true;
    }
    return false;
}
bool Config::set_user_config(std::string_view name) const noexcept{
    if(configs_.contains(name)){
        user_config_ = *configs_.find(name);
        return true;
    }
    return false;
}
const network::server::Config& Config::current_server_setting() const noexcept{
    return server_config_;
}
//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)