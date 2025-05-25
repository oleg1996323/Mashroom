#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include "def.h"
#include "sys/error_code.h"
#include "data/info.h"
#include "message.h"
#include <path_process.h>

namespace fs = std::filesystem;
using namespace std::string_literals;
class Capitalize{
private:
GribDataInfo result;
std::unordered_set<path::Storage<false>> in_path_;
fs::path dest_directory_;
std::string_view output_order_;
int cpus = 1;
DataFormat output_format_ = DataFormat::NONE;
std::vector<std::pair<fs::path,GribMsgDataInfo>> __write__(const std::vector<GribMsgDataInfo>& data);
const GribDataInfo& __capitalize_file__(const fs::path& file);
public:
static bool check_format(std::string_view fmt);
void execute();

ErrorCode add_in_path(const path::Storage<false>& path){
    if(path.path_.empty())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"empty path",AT_ERROR_ACTION::CONTINUE);
    switch(path.type_){
        case path::TYPE::FILE:
            if(!fs::exists(path.path_))
                return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else if(!fs::is_regular_file(path.path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_FILE,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else in_path_.insert(path);
            break;
        case path::TYPE::DIRECTORY:
            if(!fs::exists(path.path_))
                return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else if(!fs::is_directory(path.path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else in_path_.insert(path);
            break;
        case path::TYPE::HOST:
            in_path_.insert(path); //will be checked later at process
    }       
    return ErrorCode::NONE;
}
ErrorCode set_dest_dir(std::string_view dest_directory){
    if(fs::path(dest_directory).has_extension())
        return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    if(!fs::exists(dest_directory))
        if(!fs::create_directories(dest_directory))
            return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    dest_directory_=dest_directory;
    return ErrorCode::NONE;
}
ErrorCode set_output_order(std::string_view order){
    if(!check_format(order))
        return ErrorPrint::print_error(ErrorCode::INVALID_CAPITALIZE_ORDER,"",AT_ERROR_ACTION::CONTINUE,order);
    output_order_ = order;
    return ErrorCode::NONE;
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