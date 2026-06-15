#pragma once
#include "sys/outputdatafileformats.h"
#include <variant>
#include <cstdint>
#include <cstddef>
#include "types/time_interval.h"
#include "API/grib1/include/properties.h"
#include "serialization.h"
#include <optional>
#include "web/common/msgdef.h"
#include "data/def.h"
#include "web/common/detail/transaction.h"
#include "web/client/message/application/detail/extract/extract_form.h"

namespace network{    
    template<>
    class Message<network::Client_MsgT::EXTRACT>
            :public Message<Client_MsgT::TRANSACTION>{
        ExtractForm form_;
        bool file_;
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
        Message(Message<Client_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Client_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other) = delete;
        Message(Message&& other) noexcept{
            if(this!=&other){
                *this = std::move(other);
            }
        }
        Message& operator=(const Message& other) = delete;
        Message& operator=(Message&& other) noexcept{
            form_ = std::move(other.form_);
            return *this;
        }
        void file(bool get_file) noexcept{
            file_=get_file;
        }
        bool file() const noexcept{
            return file_;
        }
        const ExtractForm& form() const noexcept{
            return form_;
        }
        void form(ExtractForm form) noexcept{
            form_=std::move(form_);
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::EXTRACT>>{
        using type = Message<network::Client_MsgT::EXTRACT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                msg.form_,
                msg.file_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::EXTRACT>>{
        using type = Message<network::Client_MsgT::EXTRACT>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<Message<Client_MsgT::TRANSACTION>&>(msg),
                msg.form_,
                msg.file_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::EXTRACT>>{
        using type = Message<network::Client_MsgT::EXTRACT>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg),
                msg.form_,
                msg.file_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::EXTRACT>>{
        using type = Message<network::Client_MsgT::EXTRACT>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                decltype(type::form_),
                decltype(type::file_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::EXTRACT>>{
        using type = Message<network::Client_MsgT::EXTRACT>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<
                Message<Client_MsgT::TRANSACTION>,
                decltype(type::form_),
                decltype(type::file_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::EXTRACT>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::EXTRACT>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::EXTRACT>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::EXTRACT>>);