#pragma once
#include "web/common/msgdef.h"
#include "web/common/detail/transaction.h"
#include <vector>

namespace network
{
    template<>
    class Message<Server_MsgT::FILE_METADATA>:public Message<Server_MsgT::TRANSACTION>{
        std::vector<char> file_data_;
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
        Message(const Message& other) = delete;
        Message(Message&& other)=default;
        Message& operator=(const Message& other) = delete;
        Message& operator=(Message&& other) noexcept = default;
        const Message<Server_MsgT::TRANSACTION>& transaction() const noexcept{
            return static_cast<const Message<Server_MsgT::TRANSACTION>&>(*this);
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::FILE_METADATA>>{
        using type = Message<network::Server_MsgT::FILE_METADATA>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::FILE_METADATA>>{
        using type = Message<network::Server_MsgT::FILE_METADATA>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::FILE_METADATA>>{
        using type = Message<network::Server_MsgT::FILE_METADATA>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::FILE_METADATA>>{
        using type = Message<network::Server_MsgT::FILE_METADATA>;
        static constexpr size_t value = []()
        {
            return 0;
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::FILE_METADATA>>{
        using type = Message<network::Server_MsgT::FILE_METADATA>;
        static constexpr size_t value = []()
        {
            return 0;
        }();
    };
}