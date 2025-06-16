#pragma once
#include <variant>
#include <utility>

namespace network::detail{
    template<typename VARIANT,typename = void>
    struct IsVariant : std::false_type{};

    template<typename... Args>
    struct IsVariant<::std::variant<Args...>> : std::true_type{};

    //auxiliairy function for the check_variant_enum_aligned for index-sequence use
    template<typename ENUM, typename VARIANT, size_t... Is>
    constexpr bool check_variant_enum_aligned_impl(::std::index_sequence<Is...>) {
        static_assert(IsVariant<VARIANT>::value, "Must be a variant!");
        return ((std::is_same_v<ENUM, std::decay_t<decltype(std::variant_alternative_t<Is + 1, VARIANT>::get_msg_t())>> && ...)) &&
        ((std::to_underlying(std::variant_alternative_t<Is + 1, VARIANT>::get_msg_t()) == Is) && ...);
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
    class MessageHandler:public VARIANT{
        static_assert(IsVariant<VARIANT>::value,"Must be variant");
        public:
        using enum_t = ENUM;
        using VARIANT::VARIANT;
        MessageHandler() = default;
        template<typename T>
        const T& get() const{
            if(std::holds_alternative<std::decay_t<T>>(*this))
                return std::get<std::decay_t<T>>(*this);
            else return std::monostate();
        }
        template<typename T>
        T& get(){
            if(std::holds_alternative<std::decay_t<T>>(*this))
                return std::get<std::decay_t<T>>(*this);
            else return std::monostate();
        }
        //if std::monostate then -1
        int msg_type() const{
            return VARIANT::index()-1;
        }
    };
}