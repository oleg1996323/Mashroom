#pragma once
#include "web/common/msgdef.h"
#include "web/common/detail/hash.h"
#include "web/common/detail/transaction.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "web/common/detail/progress_def.h"

namespace network{
    template<>
    class Message<network::Server_MsgT::PROGRESS>:public Message<Server_MsgT::TRANSACTION>{
        float progress_ = 0;
        progress::State state_;
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
        progress_(other.progress_),
        state_(other.state_){}
        Message(Message&& other) noexcept:
        Message<Server_MsgT::TRANSACTION>(other),
        progress_(other.progress_),
        state_(other.state_){}
        Message(const Message<Server_MsgT::TRANSACTION>& trans,
            float progress,
            progress::State state):
            Message<Server_MsgT::TRANSACTION>(trans),
            progress_(progress),
            state_(state){}
        Message& operator=(const Message& other) noexcept{
            Message<Server_MsgT::TRANSACTION>::operator=(other);
            progress_=other.progress_;
            state_=other.state_;
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            Message<Server_MsgT::TRANSACTION>::operator=(std::move(other));
            progress_=std::move(other.progress_);
            state_=std::move(other.state_);
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
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::PROGRESS>>{
        using type = Message<network::Server_MsgT::PROGRESS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.progress_,
                msg.state_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::PROGRESS>>{
        using type = Message<network::Server_MsgT::PROGRESS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.progress_,
                msg.state_);
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::PROGRESS>>{
        using type = Message<network::Server_MsgT::PROGRESS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.progress_,
                msg.state_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::PROGRESS>>{
        using type = Message<network::Server_MsgT::PROGRESS>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::progress_),
                decltype(type::state_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::PROGRESS>>{
        using type = Message<network::Server_MsgT::PROGRESS>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::progress_),
                decltype(type::state_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::PROGRESS>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::PROGRESS>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::PROGRESS>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::PROGRESS>>);