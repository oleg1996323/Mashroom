#pragma once
#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include "types/data_info.h"
#include "message.h"

namespace fs = std::filesystem;
using namespace std::string_literals;
class Capitalize{
private:
GribDataInfo result;
std::string_view from_file_;
std::string_view dest_directory_;
std::string_view format_output_;
DataFormat output_type_;
void __write__(const Message& msg,
    const GribMsgDataInfo& msg_info);
public:

static bool check_format(std::string_view fmt);
const GribDataInfo& execute();
void set_from_path(std::string_view from_file){
    if(!fs::exists(from_file))
        throw std::invalid_argument("File doesn't exists "s + from_file.data());
    from_file_=from_file;
}
void set_dest_dir(std::string_view dest_directory){
    if(!fs::exists(dest_directory) && !fs::create_directories(dest_directory))
        throw std::invalid_argument("Unable to create capitalize destination path "s + dest_directory.data());
    dest_directory_=dest_directory;
}
void set_output_format(std::string_view format){
    if(!check_format(format))
        throw std::invalid_argument("Invalid capitalize format");
    format_output_ = format;
}
void set_output_type(DataFormat type){
    output_type_ = type;
}
const GribDataInfo& get_result() const{
    return result;
}
void clear_result(){
    result.info_.clear();
    result.err = ErrorCodeData::NONE_ERR;
}
GribDataInfo&& release_result() noexcept{
    GribDataInfo res(std::move(result));
    return res;
}
};