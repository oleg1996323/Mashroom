#pragma once
#include "progress_base.h"
#include <network/common/message/msgdef.h>
#include <fstream>
#include "utility.h"
#include "sys/error_code.h"

//#include "blake3.h"

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>{
    server::Status status_;
    uint64_t offset_ = 0;
    size_t file_hash_ = 0;
    std::vector<char> data_;
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    offset_(other.offset_),
    file_hash_(other.file_hash_),
    data_(other.data_){}
    MessageAdditional(MessageAdditional&& other):
    status_(other.status_),
    offset_(other.offset_),
    file_hash_(other.file_hash_),
    data_(other.data_){}
    void assign_file_segment(char* ptr, size_t size) noexcept{
        data_.assign(ptr,ptr+size);
    }
    MessageAdditional& operator=(const MessageAdditional& other) {
        if(this!=&other){
            status_ = other.status_;
            offset_ = other.offset_;
            file_hash_ = other.file_hash_;
        }
        return *this;
    }
    MessageAdditional& operator=(MessageAdditional&& other) noexcept{
        if(this!=&other){
            status_ = std::move(other.status_);
            offset_ = std::move(other.offset_);
            file_hash_ = std::move(other.file_hash_);
        }
        return *this;
    }
    MessageAdditional() = default;
};

struct SendingFileInstance{
    uint64_t file_sz_ = 0;
    char* from_mapping_ = nullptr;
    uint64_t offset_ = 0;
    uint32_t chunk_ = 0;
    int fdescriptor = -1;
    ErrorCode err_ = ErrorCode::NONE;

    SendingFileInstance(const fs::path& filename,uint32_t chunk,uint64_t offset, uint64_t size = std::numeric_limits<uint64_t>::max());
    ~SendingFileInstance();
};

bool next_chunk(SendingFileInstance& a_t);
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.offset_,msg.file_hash_,msg.data_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.offset_,msg.file_hash_,msg.data_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.offset_,msg.file_hash_,msg.data_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_),decltype(type::offset_),decltype(type::file_hash_),decltype(type::data_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_),decltype(type::offset_),decltype(type::file_hash_),decltype(type::data_)>();
        }();
    };
}