#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"

namespace network{
template<>
struct MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>{
    server::Status status_;
    std::string filename_;
    uintmax_t file_sz_ = 0;      //size of file

    MessageAdditional(ErrorCode& err,const fs::path& file_path, server::Status status);
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_EXTRACT>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.filename_,msg.file_sz_);
        }
    };
}