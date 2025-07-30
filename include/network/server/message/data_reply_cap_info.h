#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>
{
    server::Status status_;
    std::string filename;
    uintmax_t file_sz_ = 0;      //size of file
    public:
    MessageAdditional(ErrorCode& err,const fs::path& file_path, server::Status status){}
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    filename(other.filename),
    file_sz_(other.file_sz_){}
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.filename,msg.file_sz_);
        }
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE_REF>>);
