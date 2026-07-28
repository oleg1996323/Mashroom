#pragma once
#include "error_code.h"
#include "err_msg.h"
#include <string>
#include <format>
#include <iostream>
#include <print>
#include "concepts.h"

namespace ErrorPrint{
    std::string prompt(String auto&& txt){
        using namespace std::string_literals;
        if constexpr(std::is_same_v<std::decay_t<decltype(txt)>,const char*>)
            return prompt(std::string_view(txt));
        else
            return txt.empty()?"":" Prompt: "s+std::string(txt);
    }
    template<typename... Args>
    std::string message(ErrorCode err,String auto&& prompt_txt,  Args&&... args){
        std::string res = std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(args...));
        if constexpr(std::is_same_v<std::decay_t<decltype(prompt_txt)>,const char*>)
            return message(err,std::string_view(prompt_txt),std::forward<Args>(args)...);
        else{
            if(prompt_txt.empty())
                return res;
            else return res+". "+prompt(prompt_txt)+".";
        }
    }

    std::string message(ErrorCode err,String auto&& prompt_txt){
        return std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(""))+prompt(std::forward<decltype(prompt_txt)>(prompt_txt));
    }

    template<typename... Args>
    ErrorCode print_error(ErrorCode err,String auto&& prompt_txt, AT_ERROR_ACTION at_error,  Args&&... args){
        std::cerr<<message(err,std::forward<decltype(prompt_txt)>(prompt_txt),std::forward<Args>(args)...)+(at_error==AT_ERROR_ACTION::ABORT?" Abort.":"")<<std::endl;
        if(at_error==AT_ERROR_ACTION::ABORT)
            exit((uint)err);
        return err;
    }
}