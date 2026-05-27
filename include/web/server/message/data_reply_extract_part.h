#pragma once
#include "web/common/message/msgdef.h"
#include "progress_base.h"

namespace network{
template<>
struct Message<network::Server_MsgT::DATA_REPLY_EXTRACT>{
    server::Status status_;
    std::string filename_;
    uintmax_t file_sz_ = 0;      //size of file

    Message(const fs::path& file_path, server::Status status);
    Message(const Message& other):
    status_(other.status_),
    filename_(other.filename_),
    file_sz_(other.file_sz_){}
    Message(Message&& other):
    status_(other.status_),
    filename_(std::move(other.filename_)),
    file_sz_(other.file_sz_){}
    Message& operator=(const Message& other) {
        if(this!=&other){
            status_ =other.status_;
            filename_ = other.filename_;
            file_sz_ = other.file_sz_;
        }
        return *this;
    }
    Message& operator=(Message&& other) noexcept{
        if(this!=&other){
            status_ = std::move(other.status_);
            filename_ = std::move(other.filename_);
            file_sz_ = std::move(other.file_sz_);
        }
        return *this;
    }
    Message() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_),decltype(type::filename_),decltype(type::file_sz_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_),decltype(type::filename_),decltype(type::file_sz_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);