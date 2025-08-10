#pragma once
#include <variant>
#include <utility>
#include "network/common/message/message.h"

namespace network{
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
    template<typename ENUM, size_t NUM_MSG,typename... ARGS>
    constexpr bool check_variant_enum(){
        return check_variant_enum_complete<ENUM,std::variant<ARGS...>,NUM_MSG>() && check_variant_enum_aligned<ENUM,std::variant<ARGS...>>();
    }

    //dynamic polymorphism factory (for OsterMath)
    /* 
    template<class... _Types>
    struct MessageVariantFactory
    {
        static constexpr size_t kTypeCount = sizeof...(_Types);
        using VariantType = std::variant<_Types...>;
        using ResultType = std::optional<VariantType>;

        bool emplace(ResultType& result, int type) const
        {
            return emplaceVariant(result, type);
        }

    protected:
        template <std::size_t I = std::variant_size_v<VariantType>/2>
        static bool emplaceVariant(ResultType& result, std::size_t index)
        {
            if constexpr (I >= kTypeCount)
            {
                return false;
            }
            else
            {
                if (index > I)
                    return emplaceVariant<I+(std::variant_size_v<VariantType>-I)/2>(result,index);
                else if(index < I)
                    return emplaceVariant<I/2>(result,index);
                else{
                    result.emplace(std::in_place_index<I>);
                    return true;
                }
            }
        }
    }; */

    template <typename T>
    concept HasError = requires(T t) { t.error(); };

    template<typename VARIANT,typename = void>
    struct MessageVariantFactory: std::false_type{};

    template<class... Types>
    struct MessageVariantFactory<std::variant<Types...>>{
        using VariantType = std::variant<Types...>;
        using ResultType = VariantType;

        template<typename... ARGS>
        static ErrorCode emplace(ResultType& result, size_t index,ARGS&&... args) {
            if (index >= sizeof...(Types))
                return ErrorPrint::print_error(
                    ErrorCode::INVALID_ARGUMENT,"invalid variant type",AT_ERROR_ACTION::CONTINUE);
            return emplaceImpl(result, index, std::make_index_sequence<sizeof...(Types)>{},std::forward<ARGS>(args)...);
        }
    private:
        template <size_t... Is,typename... ARGS>
        static ErrorCode emplaceImpl(ResultType& result, size_t index, std::index_sequence<Is...>,ARGS&&... args) noexcept{
            ErrorCode err = ErrorCode::NONE;
            auto return_error = [&](const auto& emplaced) mutable noexcept{
                using Type = std::decay_t<decltype(emplaced)>;
                if constexpr(HasError<Type>){
                    err = emplaced.error();
                    if(err!=ErrorCode::NONE)
                        result.template emplace<0>();
                    return err;
                }
                else return ErrorCode::NONE;
            };
            auto try_emplace = [&]<size_t ID>(){
                using Type = std::variant_alternative_t<ID, VariantType>;
                if(index!=ID)
                    return ErrorCode::INVALID_ARGUMENT;
                if constexpr (sizeof...(ARGS)>0 && std::is_constructible_v<Type,ARGS...>)
                    return return_error(result.template emplace<ID>(std::forward<ARGS>(args)...));
                else if constexpr(sizeof...(ARGS)==0 && std::is_default_constructible_v<Type>)
                    return return_error(result.template emplace<ID>());
                else return ErrorCode::INVALID_ARGUMENT;
            };
            ((err!=ErrorCode::NONE?(err = try_emplace.template operator()<Is>()):(err=err)),...);
            return err;
        }
    };

    template<typename ENUM,typename = void>
    requires std::is_enum_v<ENUM>
    class _MessageHandler:std::false_type{};

    template<typename ENUM,typename... Ts>
    requires std::is_enum_v<ENUM>
    class _MessageHandler<ENUM,std::variant<Ts...>>:public std::variant<Ts...>{
        public:
        using variant_t = std::variant<Ts...>;
        using enum_t = ENUM;
        static_assert(network::check_variant_enum<ENUM,std::variant_size_v<variant_t>-1,Ts...>());
        protected:
        using factory = MessageVariantFactory<variant_t>;
        using variant_t::variant;
        template<auto T>
        requires MessageEnumConcept<T>
        const Message<T>& get() const{
            return std::get<Message<T>>(*this);
        }
        template<auto T>
        requires MessageEnumConcept<T>
        Message<T>& get() noexcept{
            return std::get<Message<T>>(*this);
        }
        //if std::monostate then -1
        int msg_type() const{
            return variant_t::index()-1;
        }

        template<typename... ARGS>
        ErrorCode emplace_message_by_id(size_t msg_t,ARGS&&... args) noexcept{
            if(msg_t+1>std::variant_size_v<variant_t>)
                return ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid variant type",AT_ERROR_ACTION::CONTINUE);
            return factory::emplace(*this,msg_t+1,std::forward<ARGS>(args)...);
        }
    };

    template<Side S>
    class MessageProcess;

    template<Side S>
    class MessageHandler:public _MessageHandler<typename network::MESSAGE_ID<S>::type,typename network::list_message<S>::type>{
        public:
        using _handler = _MessageHandler<typename network::MESSAGE_ID<S>::type, typename network::list_message<S>::type>;
        using _handler::_MessageHandler;
        private:
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
        template<auto MSG,typename... ARGS>
        requires MessageEnumConcept<MSG>
        ErrorCode emplace_message(ARGS&&... args){
            return this->template emplace<Message<MSG>>(std::forward<ARGS>(args)...).error();
        }
        template<typename... ARGS>
        ErrorCode emplace_message_by_id(size_t id, ARGS&&... args){
            return _handler::emplace_message_by_id(id,std::forward<ARGS>(args)...);
        }
        ErrorCode emplace_default_message_by_id(size_t id){
            return _handler::emplace_message_by_id(id);
        }
        void clear(){
            this->template emplace<std::monostate>();
        }
        bool has_message() const{
            if(!std::holds_alternative<std::monostate>(*this))
                return true;
            else return false;
        }
        
        template<bool NETWORK_ORDER>
        std::expected<uint64_t,serialization::SerializationEC> data_size_from_buffer(std::span<const char> buffer) noexcept{
            assert(buffer.size()>=undefined_msg_type_min_required_size<S>());
            auto visitor = [&buffer](auto& arg) mutable noexcept -> std::expected<uint64_t,serialization::SerializationEC>
            {
                using type = decltype(arg);
                if constexpr (!Data_Size_Buffer<type>)
                    return std::unexpected(serialization::SerializationEC::UNMATCHED_TYPE);
                else return arg.data_size_buffer();
            };
            return std::visit(visitor,*this);
        }
    };
}

static_assert(IsStdVariant<network::MessageHandler<network::Side::SERVER>::_handler>);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::SERVER>::_handler>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);

// namespace serialization{
//     using namespace network;
//     template<bool NETWORK_ORDER,Side S>
//     struct Serialize<NETWORK_ORDER,MessageHandler<S>>{
//         using type = MessageHandler<S>;
//         SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
//             return serialize<NETWORK_ORDER,typename type::_handler>(msg,buf);
//         }
//     };

//     template<bool NETWORK_ORDER, Side S>
//     struct Deserialize<NETWORK_ORDER,MessageHandler<S>>{
//         using type = MessageHandler<S>;
//         SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
//             return deserialize<NETWORK_ORDER,typename type::_handler>(msg,buf);
//         }
//     };
//     static_assert(serialization::deserialize_concept<true,network::MessageHandler<Side::SERVER>>);
//     static_assert(serialization::deserialize_concept<false,network::MessageHandler<Side::SERVER>>);
//     static_assert(serialization::deserialize_concept<true,network::MessageHandler<Side::CLIENT>>);
//     static_assert(serialization::deserialize_concept<false,network::MessageHandler<Side::CLIENT>>);
//     template<Side S>
//     struct Serial_size<MessageHandler<S>>{
//         using type = MessageHandler<S>;
//         size_t operator()(const type& msg) const noexcept{
//             return serial_size<typename type::_handler>(msg);
//         }
//     };

//     template<Side S>
//     struct Min_serial_size<MessageHandler<S>>{
//         using type = MessageHandler<S>;
//         static constexpr size_t value = []() ->size_t
//         {
//             return min_serial_size<typename type::_handler>();
//         }();
//     };

//     template<Side S>
//     struct Max_serial_size<MessageHandler<S>>{
//         using type = MessageHandler<S>;
//         static constexpr size_t value = []() ->size_t
//         {
//             return max_serial_size<typename type::_handler>();
//         }();
//     };
// }