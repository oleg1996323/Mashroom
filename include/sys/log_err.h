#pragma once
#include <fstream>
#include <chrono>
#include <filesystem>
#include <iostream>
#include "error_code.h"
#include "err_msg.h"
constexpr const char* log_path = LOG_DIR;

namespace fs = std::filesystem;
namespace chrono = std::chrono;

class LogError:public std::ofstream{
    public:
    LogError(){
        fs::path filename = fs::path(log_path)/(std::format("{:%Y_%m_%d_%H_%M_%S}",chrono::system_clock::now())+".txt");
        open(filename,std::ios::trunc);
        if(!is_open()){
            std::cerr<<"Cannot open "<<filename<<"\nAbort."<<std::endl;
            exit(1);
        }
    }

    static std::string get_log_time(){
        return std::format("{:%Y/%m/%d %H:%M:%S}",chrono::system_clock::now());
    }

    template<typename... Args>
    static std::string message(ErrorCodeLog err, Args&&... args){
        return std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(args...));
    }

    static std::string prompt(const std::string& txt){
        return std::string("Prompt: ")+txt;
    }

    template<typename... Args>
    void log(const char* fmt, Args&&... args){
        *this<<std::format(fmt,std::forward<Args>(args)...)<<std::endl;
    }
};