#pragma once
#include "web/common/msgdef.h"
#include "metadata.h"
#include <ranges>

namespace network
{
    template<>
    class Message<Server_MsgT::FILE_DATA>:public Message<Server_MsgT::TRANSACTION>{
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
        public:
        Message() = default;
        Message(Message<Server_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Server_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other):
            file_data_(other.file_data_){}
        Message(Message&& other) noexcept:
            file_data_(std::move(other.file_data_)){}
        Message& operator=(const Message& other){
            if(this!=&other){
                file_data_=other.file_data_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                file_data_=std::move(other.file_data_);
            }
            return *this;
        }
        const Message<Server_MsgT::TRANSACTION>& transaction() const noexcept{
            return static_cast<const Message<Server_MsgT::TRANSACTION>&>(*this);
        }
        void data(std::vector<char> d) noexcept{
            file_data_ = std::move(d);
        }
        const std::vector<char>& data() const noexcept{
            return file_data_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::FILE_DATA>>{
        using type = Message<network::Server_MsgT::FILE_DATA>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::FILE_DATA>>{
        using type = Message<network::Server_MsgT::FILE_DATA>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::FILE_DATA>>{
        using type = Message<network::Server_MsgT::FILE_DATA>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::FILE_DATA>>{
        using type = Message<network::Server_MsgT::FILE_DATA>;
        static constexpr size_t value = []()
        {
            return 0;
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::FILE_DATA>>{
        using type = Message<network::Server_MsgT::FILE_DATA>;
        static constexpr size_t value = []()
        {
            return 0;
        }();
    };
}