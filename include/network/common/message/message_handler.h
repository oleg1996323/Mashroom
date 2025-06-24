#pragma once
#include <variant>
#include <utility>
#include "network/common/message/message.h"

namespace network::detail{
    template<typename VARIANT,typename = void>
    struct IsVariant : std::false_type{};

    template<typename... Args>
    struct IsVariant<::std::variant<Args...>> : std::true_type{};

    //auxiliairy function for the check_variant_enum_aligned for index-sequence use
    template<typename ENUM, typename VARIANT, size_t... Is>
    constexpr bool check_variant_enum_aligned_impl(::std::index_sequence<Is...>) {
        static_assert(IsVariant<VARIANT>::value, "Must be a variant!");
        return ((std::is_same_v<ENUM, std::decay_t<decltype(std::variant_alternative_t<Is + 1, VARIANT>::msg_type())>> && ...)) &&
        ((std::to_underlying(std::variant_alternative_t<Is + 1, VARIANT>::msg_type()) == Is) && ...);
    }

    //check if enum-ed message-structures are listed sequentially (std::monostate,struct{ENUM::0},struct{ENUM::1},...)
    //std::monostate is ignored due to the absence of the type_msg_ field, but must be present in the variant's type-list
    template<typename ENUM, typename VARIANT>
    constexpr bool check_variant_enum_aligned() {
        return check_variant_enum_aligned_impl<ENUM, VARIANT>(
            ::std::make_index_sequence<::std::variant_size_v<VARIANT> - 1>{}
        );
    }
    template<typename ENUM, typename VARIANT,size_t NUM_MSG>
    constexpr bool check_variant_enum_complete(){
        static_assert(NUM_MSG==std::variant_size_v<VARIANT>-1);
        return NUM_MSG==::std::variant_size_v<VARIANT>-1; 
    }
    #include <cstddef>
    template<typename ENUM, typename VARIANT,size_t NUM_MSG>
    constexpr bool check_variant_enum(){
        return check_variant_enum_complete<ENUM,VARIANT,NUM_MSG>() && check_variant_enum_aligned<ENUM,VARIANT>();
    }

    template<typename ENUM,typename VARIANT>
    requires std::is_enum_v<ENUM>
    class MessageHandler:public VARIANT{
        static_assert(IsVariant<VARIANT>::value,"Must be variant");
        static_assert(network::detail::check_variant_enum<ENUM,VARIANT,::std::variant_size_v<VARIANT>-1>);

        public:
        using enum_t = ENUM;
        using VARIANT::variant;
        template<auto T>
        requires MessageEnumConcept<T>
        const Message<T>& get() const{
            return std::get<Message<T>>(*this);
        }
        template<auto T>
        requires MessageEnumConcept<T>
        Message<T>& get(){
            return std::get<Message<T>>(*this);
        }
        //if std::monostate then -1
        int msg_type() const{
            return VARIANT::index()-1;
        }
    };
}

namespace network{
    template<Side S>
    class MessageProcess;

    template<Side S>
    class MessageHandler:public network::detail::MessageHandler<typename network::MESSAGE_ID<S>::type,typename network::list_message<S>::type>{
        private:
        using _handler = network::detail::MessageHandler<typename network::MESSAGE_ID<S>::type, typename network::list_message<S>::type>;
        using _handler::MessageHandler;
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
        template<Side Aside>
        friend class network::MessageProcess;
        template<typename MESSAGE_ID<S>::type MSG,typename... ARGS>
        ErrorCode emplace_message(ARGS&&... args){
            return this->template emplace<Message<MSG>>(std::forward<ARGS>(args)...).error();
        }
        void clear(){
            this->template emplace<std::monostate>();
        }
        bool has_message(){
            if(!std::holds_alternative<std::monostate>(*this))
                return true;
            else return false;
        }
        
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER,Side S>
    struct Serialize<NETWORK_ORDER,MessageHandler<S>>{
        using type = MessageHandler<S>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            auto visitor = [&buf](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return serialization::SerializationEC::UNMATCHED_TYPE;
                else return serialization::serialize<NETWORK_ORDER>(arg,buf);
            };
            return std::visit(visitor,msg);
        }
    };

    template<Side S>
    struct Serial_size<MessageHandler<S>>{
        using type = MessageHandler<Side::CLIENT>;
        size_t operator()(const type& msg) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return size_t(0);
                else return serialization::serial_size(arg);
            };
            return std::visit(visitor,msg);
        }
    };

    template<Side S>
    struct Min_serial_size<MessageHandler<S>>{
        using type = MessageHandler<Side::CLIENT>;
        constexpr size_t operator()(const type& msg) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return size_t(0);
                else return serialization::min_serial_size(arg);
            };
            return std::visit(visitor,msg);
        }
    };

    template<Side S>
    struct Max_serial_size<MessageHandler<S>>{
        using type = MessageHandler<S>;
        constexpr size_t operator()(const type& msg) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return size_t(0);
                else return serialization::max_serial_size(arg);
            };
            return std::visit(visitor,msg);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageHandler<Side::CLIENT>>{
        using type = MessageHandler<Side::CLIENT>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            msg.clear();
            Client_MsgT::type T{};
            if(buf.size()<min_serial_size(MessageBase<(Client_MsgT::type)0>{}))
                return SerializationEC::NONE;
            deserialize<NETWORK_ORDER>(T,buf);
            
            auto visitor = [&buf](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return serialization::SerializationEC::UNMATCHED_TYPE;
                else return serialization::deserialize<NETWORK_ORDER>(arg,buf);
            };
            
            switch(T){
                case Client_MsgT::CAPITALIZE:
                    msg.emplace_message<Client_MsgT::CAPITALIZE>();
                    break;
                case Client_MsgT::CAPITALIZE_REF:
                    msg.emplace_message<Client_MsgT::CAPITALIZE_REF>();
                    break;
                case Client_MsgT::SERVER_STATUS:
                    msg.emplace_message<Client_MsgT::SERVER_STATUS>();
                    break;
                case Client_MsgT::DATA_REQUEST:
                    msg.emplace_message<Client_MsgT::DATA_REQUEST>();
                    break;
                case Client_MsgT::TRANSACTION:
                    msg.emplace_message<Client_MsgT::TRANSACTION>();
                    break;
                default:
                    return SerializationEC::UNMATCHED_TYPE;
            }
            SerializationEC code = std::visit(visitor,msg);
            if(code!=SerializationEC::NONE)
                msg.clear();
            return code;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageHandler<Side::SERVER>>{
        using type = MessageHandler<Side::SERVER>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            msg.clear();
            Server_MsgT::type T{};
            if(buf.size()<min_serial_size(MessageBase<(Server_MsgT::type)0>{}))
                return SerializationEC::NONE;
            deserialize<NETWORK_ORDER>(T,buf);
            
            auto visitor = [&buf](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return serialization::SerializationEC::UNMATCHED_TYPE;
                else return serialization::deserialize<NETWORK_ORDER>(arg,buf);
            };
            
            switch(T){
                case Server_MsgT::DATA_REPLY_FILEINFO:
                    msg.emplace_message<Server_MsgT::DATA_REPLY_FILEINFO>();
                    break;
                case Server_MsgT::SERVER_STATUS:
                    msg.emplace_message<Server_MsgT::SERVER_STATUS>();
                    break;
                case Server_MsgT::DATA_REPLY_CAPITALIZE:
                    msg.emplace_message<Server_MsgT::DATA_REPLY_CAPITALIZE>();
                    break;
                case Server_MsgT::ERROR:
                    msg.emplace_message<Server_MsgT::ERROR>();
                    break;
                case Server_MsgT::PROGRESS:
                    msg.emplace_message<Server_MsgT::PROGRESS>();
                    break;
                case Server_MsgT::DATA_REPLY_FILEPART:
                    msg.emplace_message<Server_MsgT::DATA_REPLY_FILEPART>();
                    break;
                case Server_MsgT::VERSION:
                    msg.emplace_message<Server_MsgT::VERSION>();
                    break;
                case Server_MsgT::DATA_REPLY_CAPITALIZE_REF:
                    msg.emplace_message<Server_MsgT::DATA_REPLY_CAPITALIZE_REF>();
                    break;
                case Server_MsgT::DATA_REPLY_EXTRACT:
                    msg.emplace_message<Server_MsgT::DATA_REPLY_EXTRACT>();
                    break;
                default:
                    return SerializationEC::UNMATCHED_TYPE;
            }
            SerializationEC code = std::visit(visitor,msg);
            if(code!=SerializationEC::NONE)
                msg.clear();
            return code;
        }
    };
}