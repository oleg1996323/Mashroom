#pragma once
#include "web/common/msgdef.h"
#include "sys/error_code.h"

namespace network{
    template<>
    class Message<network::Client_MsgT::ERROR>{
        std::string description_;
        std::optional<Message<Client_MsgT::TRANSACTION>> transaction_;
        ErrorCode err_ = ErrorCode::NONE;
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
        Message(ErrorCode error_code,
                    std::string description):
                    description_(description),
                    transaction_(std::nullopt),
                    err_(error_code){}
        Message(ErrorCode error_code,
                    std::string description,
                    const Message<Client_MsgT::TRANSACTION>& transaction):
                    description_(description),
                    transaction_(transaction),
                    err_(error_code){}
        Message() = default;
        Message(const Message& other) noexcept:
            description_(other.description_),
            transaction_(other.transaction_),
            err_(other.err_){}
        Message(Message&& other) noexcept:
            description_(std::move(other.description_)),
            transaction_(std::move(other.transaction_)),
            err_(other.err_){}
        Message& operator=(const Message& other) noexcept{
            if(this!=&other){
                description_=other.description_;
                transaction_=other.transaction_;
                err_=other.err_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                description_=std::move(other.description_);
                transaction_=std::move(other.transaction_);
                err_=std::move(other.err_);
            }
            return *this;
        }
        const std::optional<Message<Client_MsgT::TRANSACTION>>& transaction() const noexcept{
            return transaction_;
        }
        const std::string& description() const noexcept{
            return description_;
        }
        ErrorCode error() const noexcept{
            return err_;
        }
        void error(ErrorCode err) noexcept{
            err_=err;
        }
        void description(std::string description) noexcept{
            description_=std::move(description);
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::ERROR>>{
        using type = Message<network::Client_MsgT::ERROR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.description_,msg.transaction_,msg.err_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::ERROR>>{
        using type = Message<network::Client_MsgT::ERROR>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.description_,msg.transaction_,msg.err_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::ERROR>>{
        using type = Message<network::Client_MsgT::ERROR>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.description_,msg.transaction_,msg.err_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::ERROR>>{
        using type = Message<network::Client_MsgT::ERROR>;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                decltype(type::description_),
                decltype(type::transaction_),
                decltype(type::err_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::ERROR>>{
        using type = Message<network::Client_MsgT::ERROR>;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                decltype(type::description_),
                decltype(type::transaction_),
                decltype(type::err_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::ERROR>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::ERROR>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::ERROR>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::ERROR>>);