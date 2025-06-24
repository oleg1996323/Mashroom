#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"

namespace network{
template<>
struct MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>{
    server::Status status_;
    std::string filename_;
    uintmax_t file_sz_ = 0;      //size of file
    size_t file_hash_ = 0;

    MessageAdditional(ErrorCode& err,const fs::path& file_path, server::Status status);
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    filename_(other.filename_),
    file_sz_(other.file_sz_){}
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>;
        SerializationEC operator()(const type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };
}