#pragma once
#include "progress_base.h"
#include "msgdef.h"
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"
//#include "blake3.h"

namespace network::server{
    namespace detail{
        class MessageHandler;
    }
template<>
class Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>:public __Message__<TYPE_MESSAGE::DATA_REPLY_FILEPART>{
    uint64_t file_part_sz_ = 0;
    uint64_t offset_ = 0;
    size_t file_hash_ = 0;
    //const size_t current_hash_ = 0; //TODO: std::hash<fs::path> is temporary solution

    struct __FILE_REPLY_ASSOCIATED_DATA__{
        uint64_t file_sz_ = 0;
        char* from_mapping_ = nullptr;
        uint32_t chunk_ = 0;
        int fdescriptor = -1;
        ErrorCode err_ = ErrorCode::NONE;
    };
    friend class ::network::server::detail::MessageHandler;
    public:
    using associated_t = __FILE_REPLY_ASSOCIATED_DATA__;
    private:
    Message(    associated_t& a_t,const fs::path& filename,
                uint64_t file_part_sz,uint64_t offset = 0, 
                uint32_t chunk=UINT16_MAX,
                server::Status status=server::Status::READY);

    Message(__Message__&& other_base);
    ErrorCode serialize_impl() const;
    ErrorCode deserialize_impl();
    public:
    ErrorCode next_chunk(associated_t& a_t);
    uint64_t offset() const;
    uint64_t file_part_size() const;
    size_t hash() const;
};
}