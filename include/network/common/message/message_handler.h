#pragma once
#include <variant>
#include <utility>
#include "message.h"

namespace network{
    //check if enum-ed message-structures are listed sequentially (std::monostate,struct{ENUM::0},struct{ENUM::1},...)
    //std::monostate is ignored due to the absence of the type_msg_ field, but must be present in the variant's type-list
    template<typename ENUM, typename VARIANT>
    requires IsStdVariant<VARIANT>
    constexpr bool check_variant_enum_aligned() {
        return [&]<size_t... Js>(const std::index_sequence<Js...>&){
            constexpr auto check = []<size_t I>(){
                if constexpr (std::is_same_v<std::variant_alternative_t<I, VARIANT>,std::monostate>)
                    return true;
                else return std::is_same_v<ENUM, decltype(std::variant_alternative_t<I, VARIANT>::msg_type())> &&
                (std::to_underlying(std::variant_alternative_t<I, VARIANT>::msg_type()) == I - 1);
            };

            return (check.template operator()<Js>() && ...);
        }(std::make_index_sequence<std::variant_size_v<VARIANT>>{});
    }
    #include <cstddef>
    
    template <typename T>
    concept HasError = requires(T t) { t.error(); };

    template<typename VARIANT,typename = void>
    struct MessageVariantFactory: std::false_type{};

    template<typename... Types>
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
    class _MessageHandler:std::false_type{};

    template<typename ENUM,typename VARIANT>
    requires (std::is_enum<ENUM>::value && IsStdVariant<VARIANT>)
    class _MessageHandler<ENUM,VARIANT>:public VARIANT{
        static_assert(std::is_move_constructible_v<VARIANT>);
        static_assert(std::is_move_assignable_v<VARIANT>);
        public:
        using enum_t = ENUM;
        using VARIANT::variant;
        static_assert(network::check_variant_enum_aligned<ENUM,VARIANT>());
        protected:
        using factory = MessageVariantFactory<VARIANT>;
        _MessageHandler() = default;
        _MessageHandler(const _MessageHandler&) = delete;
        _MessageHandler(_MessageHandler&& other) noexcept:VARIANT(std::move(other)){}
        _MessageHandler& operator=(const _MessageHandler&)=delete;
        _MessageHandler& operator=(_MessageHandler&& other) noexcept{
            if(this!=&other){
                VARIANT::swap(other);
            }
            return *this;
        }
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
            return VARIANT::index()-1;
        }

        template<typename... ARGS>
        ErrorCode emplace_message_by_id(size_t msg_t,ARGS&&... args) noexcept{
            if(msg_t+1>std::variant_size_v<VARIANT>)
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
        public:
        MessageHandler(const MessageHandler&) = delete;
        MessageHandler(MessageHandler&& other) noexcept{
            *this = std::move(other);
        }

        MessageHandler& operator=(const MessageHandler&) = delete;
        MessageHandler& operator=(MessageHandler&& other) noexcept{
            if(this!=&other){
                network::list_message<S>::type::operator=(std::move(other));
            }
            return *this;
        }
        private:
        template<auto MSG,typename... ARGS>
        requires MessageEnumConcept<MSG>
        void emplace_message(ARGS&&... args){
            this->template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
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

ENABLE_DERIVED_VARIANT(network::MessageHandler<network::Side::CLIENT>,network::list_message<network::Side::CLIENT>::type);
ENABLE_DERIVED_VARIANT(network::MessageHandler<network::Side::SERVER>,network::list_message<network::Side::SERVER>::type);

static_assert(std::is_base_of_v<network::list_message<network::Side::CLIENT>::type,network::MessageHandler<network::Side::CLIENT>>);
static_assert(std::is_base_of_v<network::list_message<network::Side::SERVER>::type,network::MessageHandler<network::Side::SERVER>>);
static_assert(IsStdVariant<network::MessageHandler<network::Side::SERVER>>);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(IsStdVariant<network::MessageHandler<network::Side::CLIENT>>);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);