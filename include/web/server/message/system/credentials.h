#pragma once
#include "web/common/msgdef.h"
#include "data/def.h"

namespace network{
    template<>
    class Message<network::Server_MsgT::CREDENTIALS>{
        uint16_t attempts_=0;
        using AccessMode = Data_a;
        AccessMode access_ = AccessMode::PUBLIC;
        bool success_ = false;
        
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
        Message(uint16_t attempts,
                AccessMode access,
                bool success):
            attempts_(attempts),
            access_(access),
            success_(success){}
        Message(const Message& other) noexcept:
            attempts_(other.attempts_),
            access_(other.access_),
            success_(other.success_){}
        Message(Message&& other) noexcept:
            attempts_(other.attempts_),
            access_(other.access_),
            success_(other.success_){}
        Message& operator=(const Message& other) noexcept{
            if(this!=&other){
                attempts_=other.attempts_;
                access_=other.access_;
                success_=other.success_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                attempts_=other.attempts_;
                access_=other.access_;
                success_=other.success_;
            }
            return *this;
        }
        AccessMode access() const noexcept{
            return access_;
        }
        bool success() const noexcept{
            return success_;
        }
        uint16_t attempts_left() const noexcept{
            return attempts_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::CREDENTIALS>>{
        using type = Message<network::Server_MsgT::CREDENTIALS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.access_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::CREDENTIALS>>{
        using type = Message<network::Server_MsgT::CREDENTIALS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.access_);
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::CREDENTIALS>>{
        using type = Message<network::Server_MsgT::CREDENTIALS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.access_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::CREDENTIALS>>{
        using type = Message<network::Server_MsgT::CREDENTIALS>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::access_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::CREDENTIALS>>{
        using type = Message<network::Server_MsgT::CREDENTIALS>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::access_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::CREDENTIALS>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::CREDENTIALS>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::CREDENTIALS>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::CREDENTIALS>>);