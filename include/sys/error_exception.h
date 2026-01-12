#pragma once
#include "error_code.h"
#include "error_print.h"
#include <stdexcept>
#include <string>
#include "concepts.h"

class ErrorException:public std::exception{
    std::string what_;
    ErrorCode error_ = ErrorCode::NONE;
    public:
    template<typename... ARGS>
    ErrorException(ErrorCode err,String auto&& prompt,ARGS&&... args):what_(ErrorPrint::message(err,std::forward<std::decay_t<decltype(prompt)>>(prompt),std::forward<ARGS>(args)...)){}

    ErrorCode error() const{
        return error_;
    }
    virtual const char* what() const noexcept override{
        return what_.c_str();
    }
};