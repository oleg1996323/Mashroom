#pragma once
#include <array>
#include <string_view>

namespace ErrorData{

enum Code{
    NONE_ERR,
    OPEN_ERROR_X1,
    READ_POS_X1,
    MEMORY_ERROR_X1,
    BAD_FILE_X1,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION,
    DATA_EMPTY
};
}

constexpr std::array<std::string_view,11> err_code_data={
    "No error",
    "Error at openning file: {}",
    "Read position error at file: {}",
    "Memory allocation error (not enough memory)",
    "Corrupted file",
    "Missing grib record",
    "Ran out of memory",
    "Could not read to end of record",
    "Ran out of data or bad file",
    "Len of grib message is inconsistent",
    "End section missed"
};

#include <string>
#include <format>
#include <iostream>

namespace ErrorDataPrint{
    std::string prompt(const std::string& txt);
    template<typename... Args>
    std::string message(ErrorData::Code err,const std::string& prompt_txt,  Args&&... args){
        std::string res = std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(args...));
        if(prompt_txt.empty())
            return res;
        else return res+". "+prompt(prompt_txt)+".";
    }

    template<typename... Args>
    ErrorData::Code print_error(ErrorData::Code err,const std::string& prompt_txt,  Args&&... args){
        std::cerr<<message(err,prompt_txt,args...)<<std::endl;
        return err;
    }
}