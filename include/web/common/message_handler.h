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
    class MessageHandler:public network::list_message<S>::type
            /* typename network::MESSAGE_ID<S>::type */
    {
        using factory = VariantFactory<typename network::list_message<S>::type>;
        public:
        using VARIANT = typename network::list_message<S>::type;
        using VARIANT::variant;
        public:
        FRIEND_TEST(NetworkMesssageHandler,ClientSide);
        FRIEND_TEST(NetworkMesssageHandler,ServerSide);
        MessageHandler(const MessageHandler&) = delete;
        MessageHandler(MessageHandler&& other) noexcept{
            *this = std::move(other);
        }

        MessageHandler& operator=(const MessageHandler&) = delete;
        MessageHandler& operator=(MessageHandler&& other) noexcept{
            if(this!=&other)
                network::list_message<S>::type::operator=(std::move(other));
            return *this;
        }
        template<auto MSG,typename... ARGS>
        requires MessageEnumConcept<MSG>
        void emplace_message(ARGS&&... args) noexcept{
            this->template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
        }
        template<typename... ARGS>
        ErrorCode emplace_message_by_id(Message_t<S> id, ARGS&&... args) noexcept{
            if(id+1>std::variant_size_v<VARIANT> ||
                !factory::emplace(*this,id+1,std::forward<ARGS>(args)...))
                return ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid variant type",AT_ERROR_ACTION::CONTINUE);
            else
                return ErrorCode::NONE;
        }
        void clear() noexcept{
            this->template emplace<std::monostate>();
        }
        bool has_message() const noexcept{
            if(!std::holds_alternative<std::monostate>(*this))
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
                return std::visit(visitor,*this);
            }
            else return std::nullopt;
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