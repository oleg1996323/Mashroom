#pragma once
#include <optional>
#include <expected>
#include <set>
#include "network/common/def.h"
#include "program/data.h"
#include "network/common/message/cashed_data.h"
#include "functional/serialization.h"
#include "buffer.h"
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
        MessageBase(size_t data_sz);
        MessageBase(MessageBase&& other) noexcept;
        MessageBase() = default;
        MessageBase& operator=(MessageBase&& other);
        ~MessageBase();
        size_t data_size() const{
            return data_sz_;
        }
        constexpr static enum_t msg_type(){
            return MSG_T;
        }
        constexpr static uint64_t min_required_defining_size(){
            return sizeof(MSG_T)+sizeof(data_sz_);
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER,auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Serialize<NETWORK_ORDER,MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,msg.type_msg_,msg.data_sz_);
        }
    };

    template<bool NETWORK_ORDER,auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Deserialize<NETWORK_ORDER,MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,msg.type_msg_,msg.data_sz_);
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.type_msg_,msg.data_sz_);
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Min_serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.type_msg_,msg.data_sz_);
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct Max_serial_size<MessageBase<MSG_T>>{
        using type = MessageBase<MSG_T>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.type_msg_,msg.data_sz_);
        }
    };
}

#include "network/client/message/additional.h"
#include "network/server/message/additional.h"

namespace network{
    template<Side S>
    class MessageProcess;

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    class Message:public __MessageBuffer__{
        using type = decltype(MSG_T);
        MessageAdditional<MSG_T> additional_;
        MessageBase<MSG_T> base_;
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
        template<Side S>
        friend class MessageProcess;
        public:
        template<typename... ADD_ARGS>
        Message(ADD_ARGS&&... add_a);
        Message()=default;
        ErrorCode error() const;
        constexpr static type msg_type(){
            return decltype(base_)::msg_type();
        }
        
        static std::expected<uint64_t,serialization::SerializationEC> data_size_from_buffer(std::span<const char> buffer){
            MessageBase<MSG_T> tmp;
            if(serialization::SerializationEC err = serialization::deserialize<true>(tmp,buffer);err!=serialization::SerializationEC::NONE)
                return std::unexpected(err);
            return tmp.data_sz_;
        }
    };

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    MessageBase<MSG_T>::MessageBase(size_t data_sz):
    data_sz_(data_sz){}

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    MessageBase<MSG_T>::MessageBase(MessageBase&& other) noexcept{
        *this=std::move(other);
    }
    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    MessageBase<MSG_T>& MessageBase<MSG_T>::operator=(MessageBase<MSG_T>&& other){
        if(this!=&other)
            data_sz_= other.data_sz_;
        return *this;
    }
    
    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    template<typename... ADD_ARGS>
    Message<MSG_T>::Message(ADD_ARGS&&... add_a):
    additional_(err_,std::forward<ADD_ARGS>(add_a)...),
    base_(serialization::serial_size(additional_)+base_.min_required_defining_size()){}

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    ErrorCode Message<MSG_T>::error() const{
        return err_;
    }


    template<Side S>
    struct list_message;

    template<>
    struct list_message<Side::CLIENT>{
        using type = std::variant<std::monostate,
                        Message<Client_MsgT::DATA_REQUEST>,
                        Message<Client_MsgT::SERVER_STATUS>,
                        Message<Client_MsgT::CAPITALIZE>,
                        Message<Client_MsgT::CAPITALIZE_REF>,
                        Message<Client_MsgT::TRANSACTION>>;
    };

    template<>
    struct list_message<Side::SERVER>{
        using type =    std::variant<std::monostate,
                        Message<Server_MsgT::DATA_REPLY_FILEINFO>,
                        Message<Server_MsgT::SERVER_STATUS>,
                        Message<Server_MsgT::DATA_REPLY_CAPITALIZE>,
                        Message<Server_MsgT::ERROR>,
                        Message<Server_MsgT::PROGRESS>,
                        Message<Server_MsgT::DATA_REPLY_FILEPART>,
                        Message<Server_MsgT::VERSION>,
                        Message<Server_MsgT::DATA_REPLY_CAPITALIZE_REF>,
                        Message<Server_MsgT::DATA_REPLY_EXTRACT>>;
    };
}

namespace serialization{
    template<bool NETWORK_ORDER,template <auto> class MsgImpl,auto MSG_T>
    requires MessageConcept<MSG_T,MsgImpl>
    struct Serialize<NETWORK_ORDER,MsgImpl<MSG_T>>{
        using type = MsgImpl<MSG_T>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,msg.base_,msg.additional_);
        }
    };

    template<bool NETWORK_ORDER,template <auto> class MsgImpl,auto MSG_T>
    requires MessageConcept<MSG_T,MsgImpl>
    struct Deserialize<NETWORK_ORDER,MsgImpl<MSG_T>>{
        using type = MsgImpl<MSG_T>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,msg.base,msg.additional_);
        }
    };

    template<template <auto> class MsgImpl,auto MSG_T>
    requires MessageConcept<MSG_T,MsgImpl>
    struct Serial_size<MsgImpl<MSG_T>>{
        using type = MsgImpl<MSG_T>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.base_,msg.additional_);
        }
    };

    template<template <auto> class MsgImpl,auto MSG_T>
    requires MessageConcept<MSG_T,MsgImpl>
    struct Min_serial_size<MsgImpl<MSG_T>>{
        using type = MsgImpl<MSG_T>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.base_,msg.additional_);
        }
    };

    template<template <auto> class MsgImpl,auto MSG_T>
    requires MessageConcept<MSG_T,MsgImpl>
    struct Max_serial_size<MsgImpl<MSG_T>>{
        using type = MsgImpl<MSG_T>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.base_,msg.additional_);
        }
    };
}