#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"

namespace network::server{
template<>
class Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>:public __Message__<TYPE_MESSAGE::DATA_REPLY_EXTRACT>{
    uint64_t fn_sz = 0;
    std::string filename;
    uintmax_t file_sz = 0;      //size of file

    Message(ProgressBase progr,const fs::path& file_path, server::Status status);
    ErrorCode serialize_impl() const;
    ErrorCode deserialize_impl();
};
}