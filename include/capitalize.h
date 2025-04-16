#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include "def.h"
#include "sys/error_code.h"
#include "types/data_info.h"
#include "message.h"

namespace fs = std::filesystem;
using namespace std::string_literals;
class Capitalize{
private:
GribDataInfo result;
fs::path from_file_;
fs::path dest_directory_;
std::string_view output_order_="ym"sv;
int cpus = 1;
DataFormat output_format_ = DataFormat::NONE;
void __write__(const Message& msg,
    const GribMsgDataInfo& msg_info);
const GribDataInfo& __capitalize_file__(const fs::path& file);
public:
static bool check_format(std::string_view fmt);
void execute();

ErrorCode set_from_path(std::string_view root_directory){
    if(!fs::exists(root_directory))
        return ErrorCode::DIRECTORY_X1_DONT_EXISTS;
    if(!fs::is_directory(root_directory))
        return ErrorCode::X1_IS_NOT_DIRECTORY;
    from_file_=root_directory;
    return ErrorCode::NONE;
}
void set_dest_dir(std::string_view dest_directory){
    if(!fs::exists(dest_directory) && !fs::create_directories(dest_directory))
        throw std::invalid_argument("Unable to create capitalize destination path "s + dest_directory.data());
    dest_directory_=dest_directory;
}
void set_output_order(std::string_view order){
    if(!check_format(order))
        throw std::invalid_argument("Invalid capitalize format");
    output_order_ = order;
}
void set_output_type(DataFormat format){
    output_format_ = format;
}
const GribDataInfo& get_result() const{
    return result;
}
DataFormat output_format() const{
    return output_format_;
}
void clear_result(){
    result.info_.clear();
    result.err = ErrorCodeData::NONE_ERR;
}
void set_using_processor_cores(int cores){
    if(cores>0 && cores<std::thread::hardware_concurrency())
        cpus=cores;
    else cpus = 1;
}
GribDataInfo release_result() noexcept{
    GribDataInfo res(std::move(result));
    return res;
}
};