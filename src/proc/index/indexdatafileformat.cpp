#include "proc/index/indexdatafileformat.h"

const std::unordered_map<IndexDataFileFormats::token,std::string> extension_txt_ = {
    {IndexDataFileFormats::token::BINARY,".bin"},
    {IndexDataFileFormats::token::GRIB,".grib"},
    {IndexDataFileFormats::token::HGT,".hgt"},
    {IndexDataFileFormats::token::IEEE,".ieee"},
    {IndexDataFileFormats::token::JSON,".json"},
    {IndexDataFileFormats::token::NETCDF,".nc"},
    {IndexDataFileFormats::token::XML,".xml"}
};

#include <stdexcept>
std::string token_to_extension(IndexDataFileFormats::token token){
    if(!extension_txt_.contains(token))
        throw std::invalid_argument("undefined indexing format");
    return extension_txt_.at(token);
}

const std::unordered_map<IndexDataFileFormats::token,std::string>& token_extension() noexcept{
    return extension_txt_;
}