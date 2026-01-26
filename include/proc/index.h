#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include "definitions/def.h"
#include "sys/error_code.h"
#include "message.h"
#include "data/datastruct/searchdataresult.h"
#include "definitions/path_process.h"
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/systems/information/nat.hpp>
#include "proc/index/indexdatafileformat.h"
#include "proc/index/write.h"
#include "proc/index/indexoutputfileformat.h"
#include "index/index_result.h"

using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace fs = std::filesystem;
using namespace std::string_literals;

class Index{
private:
std::unordered_set<path::Storage<false>> in_path_;
std::unordered_set<path::Storage<false>> written_;
std::optional<fs::path> dest_directory_;
info_quantity file_sz_limits_=static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{};
int cpus = 1;
IndexOutputFileFormat::token output_format_ = IndexOutputFileFormat::token::BINARY;
bool host_ref_only = false;
std::pair<fs::path,std::vector<IndexResultVariant>> __write_file__(const std::vector<IndexResultVariant>& data);
std::vector<IndexResultVariant> __index_file__(const fs::path& file);
std::pair<fs::path,std::vector<IndexResultVariant>> __index_write_file__(const fs::path& file);
public:
void execute() noexcept;

ErrorCode add_in_path(const path::Storage<false>& path);
ErrorCode set_dest_dir(std::string_view dest_directory);
void set_output_format(IndexOutputFileFormat::token format){
    output_format_ = format;
}
IndexOutputFileFormat::token output_format() const{
    return output_format_;
}
void set_using_processor_cores(int cores){
    if(cores>0 && cores<std::thread::hardware_concurrency())
        cpus=cores;
    else cpus = 1;
}
void set_host_ref_only(){
    host_ref_only = true;
}
/**
 * @brief Sets the maximum allowed size when receiving part of data remotely via a host.
 */
void set_max_index_size(const info_quantity& info_sz){
    if(info_sz.value()==0)
        return;
    else file_sz_limits_ = info_sz;
}
info_quantity get_max_index_size() const{
    return file_sz_limits_;
}
};