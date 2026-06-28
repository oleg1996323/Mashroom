#pragma once
#include "web/common/msgdef.h"
#include "web/common/detail/transaction.h"
#include <string>
#include "detail/extract.h"
#include <cstdint>


namespace network{
    template<>
    class Message<network::Server_MsgT::EXTRACT>:public Message<Server_MsgT::TRANSACTION>
    {
        std::string filename_;
        uintmax_t file_sz_ = 0;      //size of file
        
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
        filename_(other.filename_),
        file_sz_(other.file_sz_){}
        Message(Message&& other):
        Message<Server_MsgT::TRANSACTION>(std::move(other)),
        filename_(std::move(other.filename_)),
        file_sz_(std::move(other.file_sz_)){}
        Message& operator=(const Message& other) {
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(other);
                filename_ = other.filename_;
                file_sz_ = other.file_sz_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                Message<Server_MsgT::TRANSACTION>::operator=(std::move(other));
                filename_ = std::move(other.filename_);
                file_sz_ = std::move(other.file_sz_);
            }
            return *this;
        }
        void file_size(uintmax_t size) noexcept{
            file_sz_=size;
        }
        uintmax_t file_size() const noexcept{
            return file_sz_;
        }
        void filename(const std::string& filename) noexcept{
            filename_=filename;
        }
        const std::string& filename() const noexcept{
            return filename_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::EXTRACT>>{
        using type = network::Message<network::Server_MsgT::EXTRACT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.filename_,
                msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::EXTRACT>>{
        using type = network::Message<network::Server_MsgT::EXTRACT>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,
                buf,
                static_cast<Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.filename_,
                msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::EXTRACT>>{
        using type = network::Message<network::Server_MsgT::EXTRACT>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Server_MsgT::TRANSACTION>&>(msg),
                msg.filename_,
                msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::EXTRACT>>{
        using type = network::Message<network::Server_MsgT::EXTRACT>;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::filename_),
                decltype(type::file_sz_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::EXTRACT>>{
        using type = network::Message<network::Server_MsgT::EXTRACT>;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                Message<Server_MsgT::TRANSACTION>,
                decltype(type::filename_),
                decltype(type::file_sz_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::EXTRACT>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::EXTRACT>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::EXTRACT>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::EXTRACT>>);