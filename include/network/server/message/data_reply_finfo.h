#pragma once
#include "progress_base.h"
#include "msgdef.h"

namespace network::server{
template<>
class Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>:public __Message__<TYPE_MESSAGE::DATA_REPLY_FILEINFO>{
    uint64_t fn_sz = 0;
    std::string filename;
    uintmax_t file_sz = 0;      //size of file
    size_t file_hash_ = 0;
    
    ErrorCode serialize_impl() const;
    ErrorCode deserialize_impl();
    Message(const fs::path& file_path, server::Status status);
};
}