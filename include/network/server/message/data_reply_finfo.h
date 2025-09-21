#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"
#include <fstream>
#include <sys/mman.h>
#include "utility.h"

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>{
    server::Status status_;
    std::string filename_;
    uint64_t offset_;
    uintmax_t file_sz_ = 0;      //size of file
    size_t file_hash_ = 0;

    MessageAdditional(server::Status status,const fs::path& file_path, uint64_t offset,uint64_t size);
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    filename_(other.filename_),
    file_sz_(other.file_sz_){}
    MessageAdditional(MessageAdditional&& other):
    status_(other.status_),
    filename_(std::move(other.filename_)),
    file_sz_(other.file_sz_){}
    MessageAdditional& operator=(const MessageAdditional& other) {
        if(this!=&other){
            status_ =other.status_;
            filename_ = other.filename_;
            file_sz_ = other.file_sz_;
        }
        return *this;
    }
    MessageAdditional& operator=(MessageAdditional&& other) noexcept{
        if(this!=&other){
            status_ = std::move(other.status_);
            filename_ = std::move(other.filename_);
            file_sz_ = std::move(other.file_sz_);
        }
        return *this;
    }
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_),decltype(type::filename_),decltype(type::file_sz_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_),decltype(type::filename_),decltype(type::file_sz_)>();
        }();
    };
}