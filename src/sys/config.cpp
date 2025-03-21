#include "sys/config.h"

namespace fs = std::filesystem;

const std::vector<std::string> Config::empty_{};

void Config::read(){
    if(!fs::exists(config_mashroom_dir))
        if(!fs::create_directories(config_mashroom_dir))
            ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,config_mashroom_dir);
    cmd_file.open(fs::path(config_mashroom_dir)/"usr.txt",std::ios::in|std::ios::out);
    if(!cmd_file.is_open())
        return;
    std::string buffer;
    while(std::getline(cmd_file,buffer)){
        std::istringstream line_stream(buffer);
        std::string internal_buffer;
        line_stream>>internal_buffer;
        std::vector<std::string>& commands = configs_[internal_buffer];
        while(line_stream>>internal_buffer)
            commands.push_back(internal_buffer);
    }
    cmd_file.close();
}

void Config::save(){
    if(!not_saved_.empty())
        write();
}

void Config::write(){
    if(!cmd_file.is_open()){
        std::cout<<"Openning "<<fs::path(config_mashroom_dir)/"usr.txt"<<std::endl;
        cmd_file.open(fs::path(config_mashroom_dir)/"usr.txt",std::ios::out|std::ios::app);
        if(!cmd_file.is_open())
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Error at writing to the config file.",AT_ERROR_ACTION::ABORT,(fs::path(config_mashroom_dir)/"usr.txt").c_str());
    }
    for(auto& [name,commands]:not_saved_){
        cmd_file<<name<<' ';
        for(const std::string& command:commands){
            cmd_file<<command<<' ';
        }
        cmd_file.seekp(-1,std::ios_base::end);
        cmd_file<<std::endl;
    }
    configs_.merge(not_saved_);
    not_saved_.clear();
}

bool Config::add_user_config(const std::string& name,const std::vector<std::string>& commands){
    if(!not_saved_.contains(name)){
        not_saved_[name] = commands;
        save();
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::ALREADY_EXISTING_CONFIG_NAME,"",AT_ERROR_ACTION::ABORT,name);
    return false;
}

bool Config::add_user_config(std::string_view name,const std::vector<std::string_view>& commands){
    if(!not_saved_.contains(name.data())){
        not_saved_[name.data()] = std::vector<std::string>({commands.begin(),commands.end()});
        save();
        return true;
    }
    else ErrorPrint::print_error(ErrorCode::ALREADY_EXISTING_CONFIG_NAME,"",AT_ERROR_ACTION::ABORT,name);
    return false;
}

void Config::remove_user_config(const std::string& name){
    if(has_config_name(name))
        configs_.erase(name);
}

void Config::remove_user_config(std::string_view name){
    if(has_config_name(name))
        configs_.erase(name.data());
}

const std::unordered_map<std::string,std::vector<std::string>> Config::get_user_configs() const{
    return configs_;
}
const std::vector<std::string>& Config::get_user_config(const std::string& name) const{
    if(has_config_name(name))
        return configs_.at(name);
    else return empty_;
}
const std::vector<std::string>& Config::get_user_config(std::string_view name) const{
    if(has_config_name(name))
        return configs_.at(name.data());
    else return empty_;
}
//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)