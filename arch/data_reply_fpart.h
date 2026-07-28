#pragma once
#include "web/common/msgdef.h"
#include <fstream>
#include "OsterLib/network/utility.h"
#include "sys/error.h"
#include "web/server/message/file/data_reply_finfo.h"

//#include "blake3.h"

namespace network{
template<>
struct Message<network::Server_MsgT::DATA_REPLY_FILEPART>{
    Message<Server_MsgT::DATA_REPLY_FILEINFO> meta_;
    std::vector<char> data_;
    Message(const Message& other):
    meta_(other.meta_),
    data_(other.data_){}
    Message(Message&& other):
    meta_(other.meta_),
    data_(other.data_){}
    void assign_file_segment(char* ptr, size_t size) noexcept{
        data_.assign(ptr,ptr+size);
    }
    Message& operator=(const Message& other) {
        if(this!=&other){
            meta_ = other.meta_;
            data_ = other.data_;
        }
        return *this;
    }
    Message& operator=(Message&& other) noexcept{
        if(this!=&other){
            meta_ = std::move(other.meta_);
            data_ = std::move(other.data_);
        }
        return *this;
    }
    Message() = default;
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
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.meta_,msg.data_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.meta_,msg.data_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.meta_,msg.data_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::meta_),decltype(type::data_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::meta_),decltype(type::data_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);