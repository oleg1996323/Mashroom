#pragma once
#include "error_code.h"
#include "err_msg.h"
#include <string>
#include <format>
#include <iostream>
#include <print>

namespace ErrorPrint{
    std::string prompt(const std::string& txt);
    template<typename... Args>
    std::string message(ErrorCode err,const std::string& prompt_txt,  Args&&... args){
        std::string res = std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(args...));
        if(prompt_txt.empty())
            return res;
        else return res+". "+prompt(prompt_txt)+".";
    }

    std::string message(ErrorCode err,const std::string& prompt_txt);

    template<typename... Args>
    ErrorCode print_error(ErrorCode err,const std::string& prompt_txt, AT_ERROR_ACTION at_error,  Args&&... args){
        std::cerr<<message(err,prompt_txt,args...)+(at_error==AT_ERROR_ACTION::ABORT?" Abort.":"")<<std::endl;
        if(at_error==AT_ERROR_ACTION::ABORT)
            exit((uint)err);
        return err;
    }
}