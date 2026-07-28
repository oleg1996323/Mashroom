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
#include "sys/error.h"
#include "sys/application.h"
#include "data.h"
#include "web/server.h"
#include "web/client.h"
#include "OsterLib/concepts.h"
#include <boost/algorithm/string.hpp>
#include <CLI/CLI.hpp>

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
                ErrorPrint::print_error(mashroom::errc::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,data_dir_.c_str());
        __read_initial_data_file__();
    }
    ~Mashroom(){
        save();
    }
    static mashroom::errc read_command(std::vector<std::string>&& argv);
    bool read_command();
    network::ConnectionHandle connect(std::error_code& err,
        const std::string& host,
        network::Port port,
        const network::client::Settings& settings)
    {
        return client_.connect(
            host,
            port,
            network::Socket::Type::Stream,
            network::Protocol::TCP,
            settings,
            err);
    }
    static CLI::App& command_line() noexcept;

    template<typename network::Client_MsgT::type MSG_T, typename START_FRAME = std::monostate, typename END_FRAME = std::monostate>
    auto
        request(network::ConnectionHandle hconn, START_FRAME start, network::Message<MSG_T> data, END_FRAME end){
            network::MessageHandler<network::Side::CLIENT> handler_;
            handler_.emplace_message<MSG_T>(std::move(data));
        return client_.request<network::MessageHandler<network::Side::SERVER>>(
            hconn,
            std::forward<START_FRAME>(start),
            std::move(handler_),
            std::forward<END_FRAME>(end));
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
                ErrorPrint::print_error(mashroom::errc::COMMAND_INPUT_X1_ERROR,
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