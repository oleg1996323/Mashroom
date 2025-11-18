#include "proc/index/indexoutputfileformat.h"

const std::unordered_map<IndexOutputFileFormat::token,std::string> extension_txt_ = {
    {IndexOutputFileFormat::token::BINARY,".bin"},
    {IndexOutputFileFormat::token::JSON,".json"},
    {IndexOutputFileFormat::token::XML,".xml"}
};

#include <stdexcept>
std::string output_index_token_to_extension(IndexOutputFileFormat::token token){
    if(!extension_txt_.contains(token))
        throw std::invalid_argument("undefined indexing format");
    return extension_txt_.at(token);
}

const std::unordered_map<IndexOutputFileFormat::token,std::string>& output_index_token_extensions() noexcept{
    return extension_txt_;
}