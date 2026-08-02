#pragma once
#include <filesystem>
#include <string>
#include <thread>
#include <unordered_set>
#include "definitions/def.h"
#include "sys/error.h"
#include "grib1/message.h"
#include "data/datastruct/searchdataresult.h"
#include "Location.h"
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/systems/information/nat.hpp>
#include "proc/index/indexdatafileformat.h"
#include "proc/index/write.h"
#include "proc/index/indexoutputfileformat.h"

using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace fs = std::filesystem;
using namespace std::string_literals;

class Index{
private:
std::unordered_set<Location<false>> in_path_;
std::unordered_set<Location<false>> written_;
std::optional<fs::path> dest_directory_;
info_quantity file_sz_limits_=static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{};
int cpus = 1;
IndexOutputFileFormat::token output_format_ = IndexOutputFileFormat::token::BINARY;
bool host_ref_only_ = false;
template<Data_t TYPE,Data_f FORMAT>
std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> 
    __write_file__(
            const std::vector<data::FileMsg<TYPE,FORMAT>>& data,
            osterlib::ContextedError& ctx_err);
template<Data_t TYPE,Data_f FORMAT>
std::vector<data::FileMsg<TYPE,FORMAT>> 
    __index_file__(
            const fs::path& file,
			osterlib::ContextedError& ctx_err);
template<Data_t TYPE,Data_f FORMAT>
std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> 
    __index_write_file__(
            const fs::path& file,
			osterlib::ContextedError& ctx_err);
public:
void execute() noexcept;

std::error_code add_in_path(const Location<false>& path);
std::error_code set_dest_dir(std::string_view dest_directory);
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
    host_ref_only_ = true;
}
bool host_ref_only() const{
    return host_ref_only_;
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