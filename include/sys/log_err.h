#pragma once
#include <fstream>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <format>
#include "sys/error_code.h"
#include "sys/err_msg.h"
#include "sys/error_print.h"
constexpr const char* log_path = LOG_DIR;

namespace fs = std::filesystem;
namespace chrono = std::chrono;

#ifdef LOG_ON 
class LogError:public std::ofstream{
#else
class LogError:public std::ostream{
#endif
    public:
    #ifdef LOG_ON 
    LogError(const fs::path& log_dir = LOG_DIR){
        if(!fs::exists(log_dir))
        {
            if(!log_dir.has_extension()){
                if(!fs::create_directories(log_dir))
                    ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,log_dir.c_str());
            }
            else
                ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,log_dir.c_str());
        }
        if(fs::is_regular_file(log_dir))
            ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,log_dir.c_str());
        fs::path filename = fs::path(log_dir)/(std::format("{:%Y_%m_%d_%H_%M_%S}",chrono::system_clock::now())+".txt");
        #ifdef LOG_ON 
        open(filename,std::ios::trunc);
        if(!is_open())
            ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,filename.c_str());
        #endif
    }
    #endif

    LogError(){
        set_rdbuf(std::clog.rdbuf());
    }

    ~LogError(){
        flush();
        #ifdef LOG_ON 
            close();
        #endif
    }

    template<typename... Args>
    LogError& record_log(ErrorCodeLog err,const std::string& prompt_txt,Args&&... args){
        *this<<get_log_time()<<'\n'<<message(err,prompt_txt,args...)<<std::endl;
        return *this;
    }

    static std::string get_log_time(){
        return std::format("{:%Y/%m/%d %H:%M:%S}",chrono::system_clock::now());
    }

    template<typename... Args>
    static std::string message(ErrorCodeLog err,const std::string& prompt_txt,Args&&... args){
        std::string res = std::vformat(std::string(err_msg_log.at((size_t)err)),std::make_format_args(args...));
        if(prompt_txt.empty())
            return res;
        else return res+". "+prompt(prompt_txt)+".";
    }

    static std::string prompt(const std::string& txt){
        return std::string("Prompt: ")+txt;
    }

    template<typename... Args>
    void log(const char* fmt, Args&&... args){
        *this<<std::format(fmt,std::forward<Args>(args)...)<<std::endl;
    }
};