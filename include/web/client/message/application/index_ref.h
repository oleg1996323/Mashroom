#pragma once
#include <vector>
#include "web/common/msgdef.h"
#include "detail/index/base_index.h"
#include "OsterLib/types/time_interval.h"
#include "grib1/sections.h"
#include "web/common/detail/transaction.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::INDEX_REF>:
            public Message<Client_MsgT::TRANSACTION>,
            public BaseIndexRequest{
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
            bool operator==(const Message& other) const noexcept{
                return Message<Client_MsgT::TRANSACTION>::operator==(other) &&
                    BaseIndexRequest::operator==(other);
            }
        #endif
        Message() = default;
        Message(Message<Client_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Client_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other) noexcept:
        Message<Client_MsgT::TRANSACTION>(other),
        BaseIndexRequest(other){}
        Message(Message&& other) noexcept:
        Message<Client_MsgT::TRANSACTION>(std::move(other)),
        BaseIndexRequest(std::move(other)){}
        Message(utc_tp last_update){}
        Message& operator=(const Message& other) noexcept
        {
            if(this!=&other){
                Message<Client_MsgT::TRANSACTION>::operator=(other);
                BaseIndexRequest::operator=(other);
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept
        {
            if(this!=&other){
                Message<Client_MsgT::TRANSACTION>::operator=(std::move(other));
                BaseIndexRequest::operator=(std::move(other));
            }
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::INDEX_REF>>{
        using type = Message<network::Client_MsgT::INDEX_REF>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<const BaseIndexRequest&>(msg));
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::INDEX_REF>>{
        using type = Message<network::Client_MsgT::INDEX_REF>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<BaseIndexRequest&>(msg));
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::INDEX_REF>>{
        using type = Message<network::Client_MsgT::INDEX_REF>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                static_cast<const BaseIndexRequest&>(msg));
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::INDEX_REF>>{
        using type = Message<network::Client_MsgT::INDEX_REF>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                BaseIndexRequest>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::INDEX_REF>>{
        using type = Message<network::Client_MsgT::INDEX_REF>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                BaseIndexRequest>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::INDEX_REF>>);