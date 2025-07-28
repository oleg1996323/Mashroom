#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "error_print.h"
#include "error_code.h"
#include <type_traits>
#include <filesystem>

#include "config/server.h"
#include "config/user.h"
#include "config/system.h"
namespace fs = std::filesystem;

#include "types/time_interval.h"

class Config{   
    private:
    sys::Settings sys_settings_;
    mutable network::server::Config server_config_;
    mutable user::Settings user_config_;
    std::unordered_set<user::Settings,user::SettingsHash,user::SettingsEqual> configs_;
    std::unordered_set<network::server::Config> server_configs_;
    inline static const user::Settings empty_config_{};
    inline static const network::server::Config empty_server_config_{};
    public:
    Config(){
        read_user_config();
        read_server_config();
    }
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    ~Config(){
        save();
    }
    bool add_user_config(const user::Settings& settings);
    bool add_server_config(network::server::Config&&);
    bool setup_server_config(network::server::Config&&);
    bool change_user_config(const user::Settings& settings);
    bool remove_user_config(const std::string& name);
    bool remove_user_config(std::string_view name);
    bool remove_server_config(std::string_view name);
    bool remove_server_config(const std::string& name);
    void save();
    const sys::Settings& system_settings() const{
        return sys_settings_;
    }
    bool has_config_name(const std::string& name) const{
        return configs_.contains(name);
    }
    bool has_config_name(std::string_view name) const{
        return configs_.contains(name);
    }
    bool has_server_config(std::string_view name) const{
        return server_configs_.contains(name);
    }
    bool has_server_config(const std::string& name) const{
        return server_configs_.contains(name);
    }
    bool set_current_server_config(std::string_view name) const noexcept;
    bool set_current_server_config(const std::string& name) const noexcept;
    ErrorCode set_log_directory(const fs::path& path) noexcept;
    ErrorCode set_config_directory(const fs::path& path) noexcept;
    ErrorCode set_server_config_directory(const fs::path& path) noexcept;
    const std::unordered_set<user::Settings,user::SettingsHash,user::SettingsEqual>  get_user_configs() const;
    const user::Settings& get_user_config(const std::string& name) const;
    const user::Settings& get_user_config(std::string_view name) const;
    const user::Settings& get_current_user_config() const;
    const network::server::Config& get_server_config(std::string_view name) const;
    const network::server::Config& get_server_config(const std::string& name) const;
    const network::server::Config& get_current_server_config() const;
    const network::server::Config& current_server_setting();

    private:
    void read_user_config() noexcept;
    void read_server_config() noexcept;
    void write_user_config();
    void write_server_config();
};

//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)