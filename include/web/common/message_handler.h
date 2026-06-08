#pragma once
#include <variant>
#include <utility>
#include "message_variants.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include <gtest/gtest.h>
#include "variant.h"

namespace network{
    #include <cstddef>
    
    template <typename T>
    concept HasError = requires(T t) { t.error(); };

    template<typename ENUM,typename = void>
    class _MessageHandler:std::false_type{};

    template<Side S>
    class MessageHandler
            /* typename network::MESSAGE_ID<S>::type */
    {
        network::list_message<S>::type data_;
        using factory = VariantFactory<typename network::list_message<S>::type>;
        public:
        using VARIANT = typename network::list_message<S>::type;
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
        FRIEND_TEST(NetworkMesssageHandler,ClientSide);
        FRIEND_TEST(NetworkMesssageHandler,ServerSide);
        MessageHandler()=default;
        MessageHandler(const MessageHandler&) = delete;
        MessageHandler(MessageHandler&& other) noexcept:
        data_(move(other.data_))
        {}
        MessageHandler& operator=(const MessageHandler&) = delete;
        MessageHandler& operator=(MessageHandler&& other) noexcept{
            if(this!=&other)
                data_ = std::move(other.data_);
            return *this;
        }
        template<auto MSG,typename... ARGS>
        requires MessageEnumConcept<MSG>
        void emplace_message(ARGS&&... args) noexcept{
            data_.template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
        }
        template<typename... ARGS>
        ErrorCode emplace_message_by_id(Message_t<S> id, ARGS&&... args) noexcept{
            if(id+1>std::variant_size_v<VARIANT> ||
                !factory::emplace(data_,id+1,std::forward<ARGS>(args)...))
                return ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid variant type",AT_ERROR_ACTION::CONTINUE);
            else
                return ErrorCode::NONE;
        }
        decltype(auto) index() const noexcept{
            return data_.index();
        }
        void clear() noexcept{
            data_.template emplace<std::monostate>();
        }
        network::list_message<S>::type& data() noexcept{
            return data_;
        }
        const network::list_message<S>::type& data() const noexcept{
            return data_;
        }
        bool has_message() const noexcept{
            if(!std::holds_alternative<std::monostate>(data_))
                return true;
            else return false;
        }

        std::optional<MESSAGE_ID<S>> message_type() const noexcept{
            if(has_message()){
                auto visitor = [](const auto& varval){
                    if constexpr(!std::is_same_v<decltype(varval),std::monostate>){
                        auto def_enum_msg = []<MESSAGE_ID<S> T>(const Message<T>& msg){
                            return std::optional<MESSAGE_ID<S>>(T);
                        };
                        return def_enum_msg(varval);
                    }
                    else return std::nullopt;
                };
                return std::visit(visitor,data_);
            }
            else return std::nullopt;
        }
    };
}

namespace serialization{
    template<bool NETWORK_ORDER,network::Side S>
    struct Serialize<NETWORK_ORDER,network::MessageHandler<S>>{
        using type = network::MessageHandler<S>;
        SerializationEC operator()(const type& val, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.data_);
        }
    };

    template<bool NETWORK_ORDER,network::Side S>
    struct Deserialize<NETWORK_ORDER,network::MessageHandler<S>>{
        using type = network::MessageHandler<S>;
        SerializationEC operator()(type& val, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.data_);
        }
    };

    template<network::Side S>
    struct Serial_size<network::MessageHandler<S>>{
        using type = network::MessageHandler<S>;
        size_t operator()(const type& val) const noexcept{
            return serial_size(val.data_);
        }
    };

    template<network::Side S>
    struct Min_serial_size<network::MessageHandler<S>>{
        using type = network::MessageHandler<S>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::data_)>();
        }();
    };

    template<network::Side S>
    struct Max_serial_size<network::MessageHandler<S>>{
        using type = network::MessageHandler<S>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::data_)>();
        }();
    };
}

static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);