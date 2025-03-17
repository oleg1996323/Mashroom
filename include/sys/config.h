#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "sys/error_print.h"
#include "sys/error_code.h"
#include <sys/data_info.h>

namespace fs = std::filesystem;
constexpr const char* config_mashroom_dir = CONFIG_MASHROOM_DIR;
class Config{
    public:
    Config():p_(config_mashroom_dir){
        read();
    }

    ~Config(){
        save();
        cmd_file.close();
    }

    DataInfo get_config_data_info() const{
        return DataInfo();//TODO
    }
    bool add_user_config(std::string_view name,const std::vector<std::string_view>& commands);
    bool add_user_config(const std::string& name,const std::vector<std::string>& commands);
    void change_user_config(std::string_view name,const std::vector<std::string_view>& commands);
    void change_user_config(const std::string& name,const std::vector<std::string>& commands);
    void remove_user_config(const std::string& name);
    void remove_user_config(std::string_view name);
    void save();
    bool has_config_name(const std::string& name) const{
        return configs_.contains(name);
    }
    bool has_config_name(std::string_view name) const{
        return configs_.contains(name.data());
    }
    const std::unordered_map<std::string,std::vector<std::string>> get_user_configs() const;
    const std::vector<std::string>& get_user_config(const std::string& name) const;
    const std::vector<std::string>& get_user_config(std::string_view name) const;
    private:
    void read();
    void write();
    std::unordered_map<std::string,std::vector<std::string>> configs_;
    std::unordered_map<std::string,std::vector<std::string>> not_saved_;
    fs::path p_{config_mashroom_dir};
    static const std::vector<std::string> empty_;
    std::fstream cmd_file;
    std::fstream data_file;
};

//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)