#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/data_info.h>

namespace fs = std::filesystem;
constexpr const char* config_mashroom_dir = CONFIG_MASHROOM_DIR;
class Config{
    public:
    Config():p_(config_mashroom_dir){

        read();
    }

    DataInfo get_config_data_info(){
        return DataInfo();//TODO
    }

    void read(){

    }
    private:
    std::vector<std::vector<std::string>> commands_;
    fs::path p_{config_mashroom_dir};
    std::fstream file;
};

//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)