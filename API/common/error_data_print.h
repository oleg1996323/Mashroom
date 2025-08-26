#pragma once
#include "error_data.h"
#include <string>
#include <format>
#include <iostream>

namespace API::ErrorDataPrint{
    using namespace std::string_literals;
    inline std::string prompt(const std::string& txt) noexcept{
        return txt.empty()?"":" Prompt: "s+txt;
    }

    template<API::TYPES TYPE,typename... Args>
    std::string message(typename API::ErrorData::Code<TYPE>::value err,const char* prompt_txt,  Args&&... args) noexcept{
        std::string res = std::vformat(std::string(ErrorData::Code<TYPE>::err_code_data.at((size_t)err)),std::make_format_args(args...));
        if(strlen(prompt_txt)==0)
            return res;
        else return res+". "+prompt(prompt_txt)+".";
    }

    template<API::TYPES TYPE,typename... Args>
    std::string message(typename API::ErrorData::Code<TYPE>::value err,const std::string& prompt_txt,  Args&&... args)noexcept {
        return message(err,prompt_txt.c_str(),std::forward<Args>(args)...);
    }

    template<API::TYPES TYPE,typename... Args>
    typename API::ErrorData::Code<TYPE>::value print_error(typename API::ErrorData::Code<TYPE>::value err,const char* prompt_txt,  Args&&... args) noexcept{
        std::cerr<<message(err,prompt_txt,args...)<<std::endl;
        return err;
    }

    template<API::TYPES TYPE,typename... Args>
    typename API::ErrorData::Code<TYPE>::value print_error(typename API::ErrorData::Code<TYPE>::value err,const std::string& prompt_txt,  Args&&... args) noexcept{
        return print_error(err,prompt_txt.c_str(),std::forward<Args>(args)...);
    }
}