#pragma once
#include "web/common/msgdef.h"
#include "web/common/detail/transaction.h"
#include <string>
#include <cstdint>

namespace network{
    template<>
    class Message<network::Server_MsgT::RAWDATA_PART>:public Message<Server_MsgT::TRANSACTION>
    {
        std::vector<char> part_data_;
        crypto::SHA1 digest_;
        
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
        Message() = default;
        Message(Message<Server_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Server_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other):
        Message<Server_MsgT::TRANSACTION>(other),
        part_data_(other.part_data_)
        {
            std::memcpy(&digest_,&other.digest_,sizeof(digest_));
        }
        Message(Message&& other):
        Message<Server_MsgT::TRANSACTION>(std::move(other)),
        part_data_(std::move(other.part_data_)){
            std::memcpy(&digest_,&other.digest_,sizeof(digest_));
        }
        Message& operator=(const Message& other) {
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(other);
                part_data_ = other.part_data_;
                std::memcpy(&digest_,&other.digest_,sizeof(digest_));
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(std::move(other));
                part_data_ = std::move(other.part_data_);
                std::memcpy(&digest_,&other.digest_,sizeof(digest_));
            }
            return *this;
        }
        size_t size(uintmax_t size) const noexcept{
            return part_data_.size();
        }
        const crypto::SHA1& digest() const noexcept{
            return digest_;
        }
        void part_data(std::vector<char> part_data) noexcept{
            boost::uuids::detail::sha1 ctx;
            ctx.process_bytes(part_data.data(),part_data.size());
            ctx.get_digest(digest_);
            part_data_=std::move(part_data);
        }
        const std::vector<char>& part_data() const noexcept{
            return part_data_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::RAWDATA_PART>>{
        using type = network::Message<network::Server_MsgT::RAWDATA_PART>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.part_data_,
                msg.digest_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::RAWDATA_PART>>{
        using type = network::Message<network::Server_MsgT::RAWDATA_PART>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.part_data_,
                msg.digest_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::RAWDATA_PART>>{
        using type = network::Message<network::Server_MsgT::RAWDATA_PART>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.part_data_,
                msg.digest_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::RAWDATA_PART>>{
        using type = network::Message<network::Server_MsgT::RAWDATA_PART>;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::part_data_),
                decltype(type::digest_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::RAWDATA_PART>>{
        using type = network::Message<network::Server_MsgT::RAWDATA_PART>;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::part_data_),
                decltype(type::digest_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::RAWDATA_PART>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::RAWDATA_PART>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::RAWDATA_PART>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::RAWDATA_PART>>);