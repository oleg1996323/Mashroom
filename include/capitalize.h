#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include "def.h"
#include "sys/error_code.h"
#include "data/info.h"
#include "message.h"
#include <abstractcallholder.h>

namespace fs = std::filesystem;
using namespace std::string_literals;
class Capitalize:public AbstractCallHolder{
private:
GribDataInfo result;
fs::path from_file_;
fs::path dest_directory_;
std::string_view output_order_;
int cpus = 1;
DataFormat output_format_ = DataFormat::NONE;
std::vector<std::pair<fs::path,GribMsgDataInfo>> __write__(const std::vector<GribMsgDataInfo>& data);
const GribDataInfo& __capitalize_file__(const fs::path& file);
public:
Capitalize(Caller caller = hProgram.get()):AbstractCallHolder(std::move(caller)){}
static bool check_format(std::string_view fmt);
void execute();

ErrorCode set_from_path(std::string_view root_directory){
    if(!fs::exists(root_directory))
        return ErrorPrint::print_error(ErrorCode::DIRECTORY_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,root_directory);
    if(!fs::is_directory(root_directory))
        return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,root_directory);
    from_file_=root_directory;
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