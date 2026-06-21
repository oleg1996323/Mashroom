#pragma once
#include "web/common/msgdef.h"

#include "web/common/detail/hash.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "serialization.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::TRANSACTION>{
        /// @brief operation hash
        std::string op_hash_ = 
            boost::uuids::to_string(boost::uuids::random_generator()());
        Transaction op_status_ = Transaction::DECLINE;
        std::array<char,64> reserved_;
        friend Message<Client_MsgT::TRANSACTION> 
            get_reply(const Message<Server_MsgT::TRANSACTION>&) noexcept;
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
                return  op_hash_==other.op_hash_ &&
                        op_status_==other.op_status_ &&
                        reserved_==other.reserved_;
            }
        #endif
        public:
        Message(const Message& other) noexcept:
        op_hash_(other.op_hash_),op_status_(other.op_status_){
            std::memcpy(reserved_.data(),
                    other.reserved_.data(),
                    other.reserved_.size());
        }
        Message(Message&& other) noexcept:
        op_hash_(std::move(other.op_hash_)),
        op_status_(std::move(other.op_status_)){
            reserved_.swap(other.reserved_);
            other.reserved_.fill(0);
        }
        Message(Transaction op_status):op_status_(op_status){}
        Message() = default;
        Message& operator=(const Message& other) noexcept{
            if(this!=&other){
                op_hash_=other.op_hash_;
                op_status_=other.op_status_;
                std::memcpy(reserved_.data(),
                    other.reserved_.data(),
                    other.reserved_.size());
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                op_hash_=std::move(other.op_hash_);
                op_status_=std::move(other.op_status_);
                reserved_.swap(other.reserved_);
                other.reserved_.fill(0);
            }
            return *this;
        }
        const std::string& hash() const noexcept{
            return op_hash_;
        }
        Transaction state() const noexcept{
            return op_status_;
        }
        const Message& transaction() const noexcept{
            return *this;
        }
        Message& transaction() noexcept{
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(std::is_move_constructible_v<network::Message<network::Client_MsgT::TRANSACTION>>);