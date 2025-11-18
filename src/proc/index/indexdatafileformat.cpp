#include "proc/index/indexdatafileformat.h"

const std::unordered_map<IndexDataFileFormats::token,std::string> extension_txt_ = {
    {IndexDataFileFormats::token::BINARY,".bin"},
    {IndexDataFileFormats::token::GRIB,".grib"},
    {IndexDataFileFormats::token::HGT,".hgt"},
    {IndexDataFileFormats::token::IEEE,".ieee"},
    {IndexDataFileFormats::token::NETCDF,".nc"},
};

#include <stdexcept>
std::string index_file_token_to_extension(IndexDataFileFormats::token token){
    if(!extension_txt_.contains(token))
        throw std::invalid_argument("undefined indexing format");
    return extension_txt_.at(token);
}

const std::unordered_map<IndexDataFileFormats::token,std::string>& index_file_token_extensions() noexcept{
    return extension_txt_;
}