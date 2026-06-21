#pragma once
#include "web/common/msgdef.h"
#include "types/time_interval.h"
#include "serialization.h"
#include <optional>
#include <boost/units/systems/information.hpp>
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/quantity.hpp>
#include "web/common/detail/transaction.h"
#include "detail/index/base_index.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif
using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace network{
    template<>
    class Message<network::Client_MsgT::INDEX>:
            public Message<Client_MsgT::TRANSACTION>,
            public BaseIndexRequest
    {
        std::optional<uint64_t> info_limits_;
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
        public:
        #ifdef DEBUG
            FRIEND_TEST(NetworkMesssageHandler,MessageHandlerSerializationTest);
                        FRIEND_TEST(NetworkMesssageHandler,MessageHandlerSerializationTest);
            bool operator==(const Message& other) const noexcept{
                return Message<Client_MsgT::TRANSACTION>::operator==(other) &&
                    BaseIndexRequest::operator==(other) &&
                    info_limits_==other.info_limits_;
            }
        #endif
        public:
        Message() = default;
        Message(Message<Client_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Client_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other):
        Message<Client_MsgT::TRANSACTION>(other),
        BaseIndexRequest(other),
        info_limits_(other.info_limits_){}
        Message(Message&& other) noexcept:
        Message<Client_MsgT::TRANSACTION>(std::move(other)),
        BaseIndexRequest(std::move(other)),
        info_limits_(other.info_limits_){}
        Message(utc_tp last_update,
                info_quantity info=
                static_cast<double>(
                    std::numeric_limits<uint64_t>::max())*info_units{}):
            BaseIndexRequest(last_update),
            info_limits_(info.value()){}
        Message& operator=(const Message& other) noexcept{
            if(this!=&other){
                Message<Client_MsgT::TRANSACTION>::operator=(other);
                BaseIndexRequest::operator=(other);
                info_limits_ = other.info_limits_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                Message<Client_MsgT::TRANSACTION>::
                    operator=(std::move(other));
                BaseIndexRequest::operator=(std::move(other));
                info_limits_ = std::move(other.info_limits_);
            }
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::INDEX>>{
        using type = Message<network::Client_MsgT::INDEX>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<const BaseIndexRequest&>(msg),
                msg.info_limits_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::INDEX>>{
        using type = Message<network::Client_MsgT::INDEX>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,
                static_cast<Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<BaseIndexRequest&>(msg),
                msg.info_limits_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::INDEX>>{
        using type = Message<network::Client_MsgT::INDEX>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<const BaseIndexRequest&>(msg),
                msg.info_limits_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::INDEX>>{
        using type = Message<network::Client_MsgT::INDEX>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                BaseIndexRequest,
                decltype(type::info_limits_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::INDEX>>{
        using type = Message<network::Client_MsgT::INDEX>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                BaseIndexRequest,
                decltype(type::info_limits_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::INDEX>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::INDEX>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::INDEX>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::INDEX>>);