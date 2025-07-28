#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <cctype>
#include <ranges>
#include <fstream>
#include "sys/error_code.h"
#include "sys/err_msg.h"
#include "sys/error_print.h"
#include "sys/application.h"
#include "data.h"
#include "network/server.h"
#include "program/clients_handler.h"
#include "concepts.h"
#include <boost/algorithm/string.hpp>


namespace fs = std::filesystem;
using namespace std::string_view_literals;
constexpr std::string_view mashroom_data_info = "mashroom_data.json"sv;

class Mashroom{
    Data data_;
    std::unordered_set<fs::path> data_files_;
    std::unique_ptr<network::Server> server_;
    /** @brief Handle all clients connected to different servers at current moment
     *  @details Filled at any kind of procedures like Extract or Capitalize (inside them methods) or
     *  by permanent connection to these servers
     */
    network::ClientsHandler clients_;
    fs::path data_dir_;
    std::fstream dat_file;
    void __read_initial_data_file__();
    void __write_initial_data_file__();
    fs::path __filename__() const{
        return data_dir_/mashroom_data_info;
    }
    fs::path __crash_dir__() const{
        return fs::path(std::getenv("HOME"))/"mashroom_crash";
    }
    fs::path __crash_path__() const{
        return __crash_dir__()/mashroom_data_info;
    }
    public:
    Mashroom():data_dir_(fs::path(get_current_dir_name())/"data"){
        if(!fs::exists(data_dir_))
            if(!fs::create_directories(data_dir_))
                ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,data_dir_.c_str());
        __read_initial_data_file__();
    }
    Mashroom(Mashroom&& other):data_(std::move(other.data_)),data_files_(std::move(other.data_files_)),
        data_dir_(std::move(other.data_dir_)),dat_file(std::move(other.dat_file)){}
    ~Mashroom(){
        save();
    }
    static ErrorCode read_command(const std::vector<std::string>& argv);
    /**
     * @brief Suspend active server
     * @todo Expand to different server specified by type (data,cadastre,measurement etc)
     */
    void collapse_server(bool wait_processes = false);
    /**
     * @brief Close active server and (optionaly close instantly close all existing connection without finishing the processes)
     * @todo Expand to different server specified by type (data,cadastre,measurement etc)
     */
    void close_server(bool wait_processes = false); //further will be lot of servers (data,cadastre,measurement etc)
    void shutdown_server(bool wait_processes = false); //further will be lot of servers (data,cadastre,measurement etc)
    void deploy_server(); //further will be lot of servers (data,cadastre,measurement etc)
    void launch_server(); //further will be lot of servers (data,cadastre,measurement etc)
    bool read_command();
    ErrorCode connect(const std::string& host);

    template<network::Client_MsgT::type MSG_T, typename... ARGS>
    ErrorCode request(const std::string& host,ARGS&&... args);
    const Data& data(){
        return data_;
    }
    void add_data(GribDataInfo& data){
        data_.add_data(data);
    }
    bool save(){
        if(data_.unsaved()){
            std::cout<<"Save changes? (yes/no)\n>>";
            std::string buffer;
            std::getline(std::cin,buffer);

            if(boost::iequals(buffer,std::string_view("yes"))){
                data_.save();
                __write_initial_data_file__();
                return true;
            }
            else if(boost::iequals(buffer,std::string_view("no")))
                return false;
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"please write \"yes\" if you want to save changes; else write \"no\"",AT_ERROR_ACTION::CONTINUE,buffer);
            }
            return true;
        }
        else return false;
    }
    static Mashroom& instance(){
        static Mashroom inst;
        return inst;
    }
};

template<network::Client_MsgT::type MSG_T, typename... ARGS>
ErrorCode Mashroom::request(const std::string& host,ARGS&&... args){
    return clients_.request<MSG_T>(host,std::forward<ARGS>(args)...);
}