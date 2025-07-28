#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include "definitions/def.h"
#include "sys/error_code.h"
#include "data/sublimed_info.h"
#include "data/info.h"
#include "message.h"
#include "definitions/path_process.h"
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/systems/information/nat.hpp>
#include "sys/internaldatafileformats.h"

using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace fs = std::filesystem;
using namespace std::string_literals;
class Capitalize{
private:
GribDataInfo result;
std::unordered_set<path::Storage<false>> in_path_;
fs::path dest_directory_;
std::string_view output_order_;
info_quantity file_sz_limits_=static_cast<double>(std::numeric_limits<int32_t>::max())*info_units{};
int cpus = 1;
InternalDateFileFormats output_format_ = InternalDateFileFormats::NATIVE;
bool host_ref_only = false;
std::vector<std::pair<fs::path,GribMsgDataInfo>> __write__(const std::vector<GribMsgDataInfo>& data);
const GribDataInfo& __capitalize_file__(const fs::path& file);
public:
static bool check_format(std::string_view fmt);
void execute() noexcept;

ErrorCode add_in_path(const path::Storage<false>& path);
ErrorCode set_dest_dir(std::string_view dest_directory);
ErrorCode set_output_order(std::string_view order){
    if(!check_format(order))
        return ErrorPrint::print_error(ErrorCode::INVALID_CAPITALIZE_ORDER,"",AT_ERROR_ACTION::CONTINUE,order);
    output_order_ = order;
    return ErrorCode::NONE;
}
void set_output_format(InternalDateFileFormats format){
    output_format_ = format;
}
const GribDataInfo& get_result() const{
    return result;
}
InternalDateFileFormats output_format() const{
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
void set_host_ref_only(){
    host_ref_only = true;
}
/**
 * @brief Sets the maximum allowed size when receiving part of data remotely via a host.
 */
void set_max_cap_size(const info_quantity& info_sz){
    if(info_sz.value()==0)
        return;
    else file_sz_limits_ = info_sz;
}
};