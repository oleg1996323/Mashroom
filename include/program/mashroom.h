#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <ranges>
#include <fstream>
#include "error_code.h"
#include "err_msg.h"
#include "error_print.h"
#include "cmd_parse/functions.h"

#include "capitalize_parse.h"
#include "check_parse.h"
#include "extract_parse.h"
#include "config_parse.h"
#include "cmd_parse/cmd_translator.h"
#include "help.h"
#include "application.h"

#include "data.h"

namespace fs = std::filesystem;
using namespace std::string_view_literals;
constexpr std::string_view mashroom_data_info = "mashroom_data.json"sv;

class Mashroom{
    Data data_;
    std::unordered_set<fs::path> data_files_;
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
    static void read_command(const std::vector<std::string_view>& argv);
    bool read_command(std::istream& stream);
    const Data& data(){
        return data_;
    }
    void add_data(GribDataInfo& data){
        data_.add_data(data);
    }
    bool save(){
        if(data_.unsaved()){
            data_.save();
            __write_initial_data_file__();
            return true;
        }
        else return false;
    }
};

inline std::unique_ptr<Mashroom> hProgram;