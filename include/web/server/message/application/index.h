#pragma once
#include "web/common/msgdef.h"
#include <optional>
#include "serialization.h"
#include "web/server/message/application/detail/index.h"
#include "web/common/detail/transaction.h"

namespace network{
    template<>
    class Message<network::Server_MsgT::INDEX>:public Message<Server_MsgT::TRANSACTION>
    {
        IndexResult blocks_;
        template<bool,auto>
        friend struct serialization::Serialize;
        template<bool,auto>
        friend struct serialization::Deserialize;
        template<auto>
        friend struct serialization::Serial_size;
        template<auto>
        friend struct serialization::Min_serial_size;
        template<auto>
        friend struct serialization::Max_serial_size;
        Message() = default;
        public:
        Message(Message<Server_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Server_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other):
            Message<Server_MsgT::TRANSACTION>(other),
            blocks_(other.blocks_){}
        Message(Message&& other) noexcept:
            Message<Server_MsgT::TRANSACTION>(std::move(other)),
            blocks_(std::move(other.blocks_)){}
        Message& operator=(const Message& other){
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(other);
                blocks_=other.blocks_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(std::move(other));
                blocks_=std::move(other.blocks_);
            }
            return *this;
        }
        
        const Message<Server_MsgT::TRANSACTION>& transaction() const noexcept{
            return static_cast<const Message<Server_MsgT::TRANSACTION>&>(*this);
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::INDEX>>{
        using type = network::Message<network::Server_MsgT::INDEX>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,
                    static_cast<const network::Message<network::Server_MsgT::TRANSACTION>&>(msg),
                    msg.blocks_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::INDEX>>{
        using type = network::Message<network::Server_MsgT::INDEX>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,
                static_cast<network::Message<network::Server_MsgT::TRANSACTION>&>(msg),
                msg.blocks_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::INDEX>>{
        using type = network::Message<network::Server_MsgT::INDEX>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const network::Message<network::Server_MsgT::TRANSACTION>&>(msg),
                msg.blocks_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::INDEX>>{
        using type = network::Message<network::Server_MsgT::INDEX>;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                network::Message<network::Server_MsgT::TRANSACTION>,
                decltype(type::blocks_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::INDEX>>{
        using type = network::Message<network::Server_MsgT::INDEX>;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                network::Message<network::Server_MsgT::TRANSACTION>,
                decltype(type::blocks_)>();
        }();
    };
}

// static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::INDEX>>);
// static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::INDEX>>);