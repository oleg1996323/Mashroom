#pragma once
#include "sys/error_code.h"
#include <fstream>
#include <filesystem>
#include <iostream>

ErrorCode make_dir(const std::filesystem::path& filepath) noexcept;
template<typename DATA_T>
ErrorCode make_file(std::ofstream& file,const std::filesystem::path& out_f_name,const DATA_T& result) noexcept;


template<typename DATA_T>
ErrorCode make_file(std::ofstream& file,const std::filesystem::path& out_f_name,const DATA_T& result) noexcept{
    {
        ErrorCode err = make_dir(out_f_name);
        if(err!=ErrorCode::NONE)
            return err;
    }
    file.open(out_f_name,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        if(fs::exists(out_f_name) && fs::is_regular_file(out_f_name) && fs::status(out_f_name).permissions()>fs::perms::none){
            log().record_log(ErrorCodeLog::FILE_X1_PERM_DENIED,"",out_f_name.c_str());
            return ErrorCode::INTERNAL_ERROR;
        }   
    }
    std::cout<<"Writing to "<<out_f_name<<std::endl;
    return ErrorCode::NONE;
}