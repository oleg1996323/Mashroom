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
#include "web/server.h"
#include "web/client.h"
#include "concepts.h"
#include <boost/algorithm/string.hpp>


namespace fs = std::filesystem;
using namespace std::string_view_literals;
constexpr std::string_view mashroom_data_info = "mashroom_data.json"sv;

class Mashroom{
    Data data_;
    std::unordered_set<fs::path> data_files_;
    fs::path data_dir_;
    network::Server server_;
    network::Client client_;
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
    Mashroom():
        data_dir_(fs::path(get_current_dir_name())/"data"),
        client_([](){
            std::error_code err;
            return network::Client(
                err,
                app().config().client_config().current_settings().number_events_);
        }()){
        if(!fs::exists(data_dir_))
            if(!fs::create_directories(data_dir_))
                ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,data_dir_.c_str());
        __read_initial_data_file__();
    }
    ~Mashroom(){
        save();
    }
    static ErrorCode read_command(const std::vector<std::string>& argv);
    bool read_command();
    ErrorCode connect(const std::string& host);

    template<typename DATA_FRAME_SEND, typename START_FRAME = std::monostate, typename END_FRAME = std::monostate>
    std::shared_ptr<network::MessageHandler<network::Side::SERVER>> 
        request(network::ConnectionHandle hconn, START_FRAME &&start, DATA_FRAME_SEND &&data, END_FRAME &&end){
        return client_.request<network::MessageHandler<network::Side::SERVER>>(
            hconn,
            std::forward<decltype(start)>(start),
            std::forward<decltype(data)>(data),
            std::forward<decltype(end)>(end));
    }
    network::Server& server() noexcept{
        return server_;
    }
    const Data& data() const{
        return data_;
    }
    Data& data(){
        return data_;
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
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "please write \"yes\" if you want to save changes; \
else write \"no\"",
                AT_ERROR_ACTION::CONTINUE,buffer);
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