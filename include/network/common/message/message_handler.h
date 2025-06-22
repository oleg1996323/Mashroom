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
            std::make_index_sequence<std::variant_size_v<VARIANT> - 1>{}
        );
    }
    template<typename ENUM, typename VARIANT,size_t NUM_MSG>
    constexpr bool check_variant_enum_complete(){
        return NUM_MSG==std::variant_size_v<VARIANT>-1; 
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
        static_assert(network::detail::check_variant_enum<ENUM,VARIANT,std::variant_size_v<VARIANT>-1>);
        public:
        using enum_t = ENUM;
        using VARIANT::VARIANT;
        MessageHandler() = default;
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
        friend class network::MessageProcess<S>;
        template<MESSAGE_ID<S>::type MSG,typename... ARGS>
        ErrorCode create_message(ARGS&&... args){
            return emplace<Message<MSG>>(std::forward<ARGS>(args)...).error();
        }
    };
}