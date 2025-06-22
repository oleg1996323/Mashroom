#pragma once
#include "progress_base.h"
#include <network/common/message/msgdef.h>

namespace network{
template<>
struct MessageAdditional<Server_MsgT::PROGRESS>{
    server::Status status_;
    ProgressBase prog_;
    MessageAdditional(ErrorCode& err,const ProgressBase& progress,server::Status status):
    status_(status),prog_(progress){}
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<Server_MsgT::PROGRESS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.prog_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<Server_MsgT::PROGRESS>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.prog_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<Server_MsgT::PROGRESS>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.status_,msg.prog_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<Server_MsgT::PROGRESS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.status_,msg.prog_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<Server_MsgT::PROGRESS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.status_,msg.prog_);
        }
    };
}