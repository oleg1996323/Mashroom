#pragma once
#include "progress_base.h"
#include <network/common/message/msgdef.h>

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::PROGRESS>{
    server::Status status_;
    ProgressBase prog_;
    MessageAdditional(const ProgressBase& progress,server::Status status):
    status_(status),prog_(progress){}
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    prog_(other.prog_){}
    MessageAdditional(MessageAdditional&& other):
    status_(other.status_),
    prog_(other.prog_){}
    MessageAdditional& operator=(const MessageAdditional& other){
        if(this!=&other){
            status_ = other.status_;
            prog_ = other.prog_;
        }
        return *this;
    }
    MessageAdditional& operator=(MessageAdditional&& other) noexcept{
        if(this!=&other){
            status_ = std::move(other.status_);
            prog_ = std::move(other.prog_);
        }
        return *this;
    }
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<network::Server_MsgT::PROGRESS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.prog_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<network::Server_MsgT::PROGRESS>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.prog_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<network::Server_MsgT::PROGRESS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.prog_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<network::Server_MsgT::PROGRESS>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_),decltype(type::prog_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Server_MsgT::PROGRESS>>{
        using type = MessageAdditional<network::Server_MsgT::PROGRESS>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_),decltype(type::prog_)>();
        }();
    };
}