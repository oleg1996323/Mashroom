#pragma once
#include "web/common/msgdef.h"
#include "web/error.h"
#include "OsterLib/contexted_error.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::ERROR>{
        std::string description_;
        std::vector<osterlib::Field> fields_;
        std::optional<Message<Client_MsgT::TRANSACTION>> transaction_;
        mashroom::network::errc err_;
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
                return description_==other.description_ && 
                    transaction_==other.transaction_ &&
                    err_==other.err_;
            }
        #endif
        public:
        Message(mashroom::network::errc error_code,
                    std::string description):
                    description_(description),
                    transaction_(std::nullopt),
                    err_(error_code){}
        Message(mashroom::network::errc error_code,
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
        void add_field(std::string key,std::string value) noexcept{
            fields_.push_back(osterlib::Field(std::move(key),std::move(value)));
        }
        void add_field(osterlib::Field field) noexcept{
            fields_.push_back(std::move(field));
        }
        void add_fields(std::vector<osterlib::Field> fields) noexcept{
            if(fields.empty())
                fields_.swap(fields);
            else fields_.insert(fields_.end(),
                std::make_move_iterator(fields.begin()),
                std::make_move_iterator(fields.end()));
        }
        mashroom::network::errc error() const noexcept{
            return err_;
        }
        void error(mashroom::network::errc err) noexcept{
            err_=err;
        }
        osterlib::ContextedError contexted_error() const noexcept{
            osterlib::ContextedError result(err_,description_);
            for(auto& field:fields_)
                result.with_field(field.key(),field.value());
            return result;
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
static_assert(std::is_move_constructible_v<network::Message<network::Client_MsgT::ERROR>>);