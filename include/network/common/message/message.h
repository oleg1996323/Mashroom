#pragma once
#include <optional>
#include <expected>
#include <set>
#include "definitions.h"
#include "serialization.h"
#include "msgdef.h"

using namespace std::chrono;
namespace fs = std::filesystem;

namespace network{
    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct MessageBase{
        using enum_t = decltype(MSG_T);
        static constexpr enum_t type_msg_ = MSG_T;
        uint64_t data_sz_ = 0;
        bool msg_more_ = false;
        MessageBase(size_t data_sz);
        MessageBase(MessageBase<MSG_T>&& other) noexcept:data_sz_(other.data_sz_){}
        MessageBase(const MessageBase<MSG_T>& other) = delete;
        MessageBase& operator=(const MessageBase<MSG_T>& other) = delete;
        MessageBase& operator=(MessageBase<MSG_T>&& other);
        size_t data_size() const{
            return data_sz_;
        }
        constexpr static enum_t msg_type(){
            return MSG_T;
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    MessageBase<MSG_T>::MessageBase(size_t data_sz):
    data_sz_(data_sz){}

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    MessageBase<MSG_T>& MessageBase<MSG_T>::operator=(MessageBase<MSG_T>&& other){
        if(this!=&other){
            data_sz_= other.data_sz_;
            msg_more_ = other.msg_more_;
        }
        return *this;
    }
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER,auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Serialize<NETWORK_ORDER,MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.data_sz_,msg.msg_more_);
        }
    };

    template<bool NETWORK_ORDER,auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Deserialize<NETWORK_ORDER,MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.data_sz_,msg.msg_more_);
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.data_sz_,msg.msg_more_);
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Min_serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::data_sz_),decltype(type::msg_more_)>();
        }();
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Max_serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::data_sz_),decltype(type::msg_more_)>();
        }();
    };
}

#include "network/client/message/additional.h"
#include "network/server/message/additional.h"

namespace network{

    template<typename MSG>
    concept Data_Size_Buffer = requires(const MSG& val){
        {std::declval<MSG>().data_size_buffer()} -> std::same_as<uint64_t>;
    };

    template<Side S>
    class MessageProcess;

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    class Message{
        static_assert(std::is_move_constructible_v<MessageBase<MSG_T>>);
        static_assert(std::is_move_assignable_v<MessageBase<MSG_T>>);
        using type = decltype(MSG_T);
        MessageAdditional<MSG_T> additional_;
        MessageBase<MSG_T> base_;

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
        template<Side S>
        friend class MessageProcess;
        public:
        template<typename... ADD_ARGS>
        Message(ADD_ARGS&&... add_a);
        Message(const Message<MSG_T>&) = delete;
        Message(Message<MSG_T>&& other) noexcept:
        base_(std::move(other.base_)),
        additional_(std::move(other.additional_)){}
        Message& operator=(const Message&) = delete;
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                base_ = std::move(other.base_);
                additional_ = std::move(other.additional_);
            }
            return *this;
        }
        void set_message_more() noexcept{
            base_.msg_more_ = true;
        }
        bool message_more() const noexcept{
            return base_.msg_more_;
        }
        constexpr static type msg_type(){
            return decltype(base_)::msg_type();
        }
        
        const auto& additional() const{
            return additional_;
        }
        auto& additional(){
            return additional_;
        }
        const auto& base() const{
            return base_;
        }
    };
    
    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    template<typename... ADD_ARGS>
    Message<MSG_T>::Message(ADD_ARGS&&... add_a):
    additional_(std::forward<ADD_ARGS>(add_a)...),
    base_(serialization::serial_size(additional_)){}

    template<Side S>
    struct list_message;

    template<>
    struct list_message<Side::CLIENT>{
        using type = std::variant<std::monostate,
                        Message<network::Client_MsgT::DATA_REQUEST>,
                        Message<network::Client_MsgT::SERVER_STATUS>,
                        Message<network::Client_MsgT::INDEX>,
                        Message<network::Client_MsgT::INDEX_REF>,
                        Message<network::Client_MsgT::TRANSACTION>>;
    };
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::DATA_REQUEST>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::DATA_REQUEST>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::INDEX>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::INDEX>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::INDEX_REF>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::INDEX_REF>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::TRANSACTION>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::TRANSACTION>>);

    template<>
    struct list_message<Side::SERVER>{
        using type =    std::variant<std::monostate,
                        Message<network::Server_MsgT::DATA_REPLY_FILEINFO>,
                        Message<network::Server_MsgT::SERVER_STATUS>,
                        Message<network::Server_MsgT::DATA_REPLY_INDEX>,
                        Message<network::Server_MsgT::ERROR>,
                        Message<network::Server_MsgT::PROGRESS>,
                        Message<network::Server_MsgT::DATA_REPLY_FILEPART>,
                        Message<network::Server_MsgT::VERSION>,
                        Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>,
                        Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>;
    };
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_FILEINFO>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_FILEINFO>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::ERROR>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::ERROR>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::PROGRESS>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::PROGRESS>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::VERSION>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::VERSION>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);

    constexpr Side get_side(MessageEnumConcept_t auto msg_t){
        using type = decltype(msg_t);
        if constexpr(std::is_same_v<type,MESSAGE_ID<Side::SERVER>::type>)
            return MESSAGE_ID<Side::SERVER>::side();
        else if constexpr(std::is_same_v<type,MESSAGE_ID<Side::CLIENT>::type>)
            return MESSAGE_ID<Side::CLIENT>::side();
        else static_assert(false,"Undefined side");
    }
}

namespace serialization{
    template<bool NETWORK_ORDER,auto MSG_T>
    struct Serialize<NETWORK_ORDER,network::Message<MSG_T>>{
        using type = network::Message<MSG_T>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.base_,msg.additional_);
        }
    };

    template<bool NETWORK_ORDER,auto MSG_T>
    struct Deserialize<NETWORK_ORDER,network::Message<MSG_T>>{
        using type = network::Message<MSG_T>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.base_,msg.additional_);
        }
    };

    template<auto MSG_T>
    struct Serial_size<network::Message<MSG_T>>{
        using type = network::Message<MSG_T>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.base_,msg.additional_);
        }
    };

    template<auto MSG_T>
    struct Min_serial_size<network::Message<MSG_T>>{
        using type = network::Message<MSG_T>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::base_),decltype(type::additional_)>();
        }();
    };

    template<auto MSG_T>
    struct Max_serial_size<network::Message<MSG_T>>{
        using type = network::Message<MSG_T>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::base_),decltype(type::additional_)>();
        }();
    };
}