#pragma once
#include <variant>
#include <utility>
#include "message_variants.h"
#include "sys/error.h"
#include <gtest/gtest.h>
#include "OsterLib/variant.h"

namespace network{
    #include <cstddef>

    template<Side S>
    class MessageHandler
            /* typename network::MESSAGE_ID<S>::type */
    {
        network::list_message<S>::type data_;
        public:
        using VARIANT = typename network::list_message<S>::type;
        using VARIANT_SYS = typename MessageCategory<S,MessageCategoryEnum::SYSTEM>::type;
        using VARIANT_APP = typename MessageCategory<S,MessageCategoryEnum::APPLICATION>::type;
        using VARIANT_FILE = typename MessageCategory<S,MessageCategoryEnum::FILE>::type;
        private:
        using factory_cat = VariantFactory<VARIANT>;
        using factory_sys = VariantFactory<VARIANT_SYS>;
        using factory_app = VariantFactory<VARIANT_APP>;
        using factory_file = VariantFactory<VARIANT_FILE>;

        static_assert(std::is_same_v<std::variant_alternative_t<0,VARIANT_SYS>,
                        std::monostate>);
        static_assert(std::is_same_v<std::variant_alternative_t<0,VARIANT_FILE>,
                        std::monostate>);
        static_assert(std::is_same_v<std::variant_alternative_t<0,VARIANT_APP>,
                        std::monostate>);
        public:
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
        MessageHandler(const MessageHandler& other) noexcept:
        data_(other.data_){}
        MessageHandler(MessageHandler&& other) noexcept:
        data_(move(other.data_)){}
        MessageHandler& operator=(const MessageHandler& other) noexcept{
            if(this!=&other){
                data_ = other.data_;
            }
            return *this;
        }
        MessageHandler& operator=(MessageHandler&& other) noexcept{
            if(this!=&other)
                data_ = std::move(other.data_);
            return *this;
        }
        template<typename MESSAGE_ID<S>::type MSG,typename... ARGS>
        Message<MSG>& emplace_message(ARGS&&... args) noexcept{
            if constexpr(is_app_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_APP>();
                return cat_data.template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
            }
            else if constexpr(is_sys_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_SYS>();
                return cat_data.template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
            }
            else if constexpr(is_file_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_FILE>();
                return cat_data.template emplace<Message<MSG>>(std::forward<ARGS>(args)...);
            }
            else static_assert(false,"Not implemented");
        }
        template<typename MESSAGE_ID<S>::type MSG,typename... ARGS>
        Message<MSG>& emplace_message(Message<MSG> msg) noexcept{
            if constexpr(is_app_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_APP>();
                return cat_data.template emplace<Message<MSG>>(std::move(msg));
            }
            else if constexpr(is_sys_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_SYS>();
                return cat_data.template emplace<Message<MSG>>(std::move(msg));
            }
            else if constexpr(is_file_message_v<S,MSG>){
                auto& cat_data = data_.template emplace<VARIANT_FILE>();
                return cat_data.template emplace<Message<MSG>>(std::move(msg));
            }
            else static_assert(false,"Not implemented");
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
        bool is_system() const noexcept{
            return std::holds_alternative<SystemMsg<S>>(data_);
        }
        bool is_application() const noexcept{
            return std::holds_alternative<AppMsg<S>>(data_);
        }
        bool is_file() const noexcept{
            return std::holds_alternative<FileMsg<S>>(data_);
        }
        bool has_message() const noexcept{
            if(!std::holds_alternative<std::monostate>(data_)){
                auto visit = [](const auto& msg) noexcept 
                ->bool
                {
                    using type = std::decay_t<decltype(msg)>;
                    if constexpr(std::is_same_v<type,std::monostate>)
                        return false;
                    else return true;
                };
                return std::visit(visit,data_);
            }
            else return false;
        }

        template<typename MESSAGE_ID<S>::type MSG_T>
        std::optional<std::reference_wrapper<const Message<MSG_T>>>
            get_message() const noexcept
        {
            if constexpr (is_app_message_v<S,MSG_T>){
                if(std::holds_alternative<
                        typename MessageCategory<S,
                            MessageCategoryEnum::APPLICATION>::type>(data_))
                {
                    const auto& cat = std::get<
                                typename MessageCategory<
                                S,MessageCategoryEnum::APPLICATION>::type>(data_);
                    if(std::holds_alternative<Message<MSG_T>>(cat))
                        return std::cref(std::get<Message<MSG_T>>(cat));
                    else return std::nullopt;
                }
                else return std::nullopt;
            }
            else if constexpr (is_sys_message_v<S,MSG_T>){
                if(std::holds_alternative<
                        typename MessageCategory<S,
                            MessageCategoryEnum::SYSTEM>::type>(data_))
                {
                    const auto& cat = std::get<
                                typename MessageCategory<
                                S,MessageCategoryEnum::SYSTEM>::type>(data_);
                    if(std::holds_alternative<Message<MSG_T>>(cat))
                        return std::cref(std::get<Message<MSG_T>>(cat));
                    else return std::nullopt;
                }
                else return std::nullopt;
            }
            else if constexpr (is_file_message_v<S,MSG_T>){
                if(std::holds_alternative<
                        typename MessageCategory<S,
                            MessageCategoryEnum::FILE>::type>(data_))
                {
                    const auto& cat = std::get<
                                typename MessageCategory<
                                S,MessageCategoryEnum::FILE>::type>(data_);
                    if(std::holds_alternative<Message<MSG_T>>(cat))
                        return std::cref(std::get<Message<MSG_T>>(cat));
                    else return std::nullopt;
                }
                else return std::nullopt;
            }
            else static_assert(false,"not implemented category");
        }

        std::optional<typename MESSAGE_ID<S>::type> message_type() const noexcept{
            if(has_message()){
                auto cat_visitor = [](const auto& category) noexcept ->
                    std::optional<typename MESSAGE_ID<S>::type>
                {
                    if constexpr(
                        !std::is_same_v<std::decay_t<decltype(category)>,std::monostate>)
                    {
                        auto msg_visitor=[](const auto& message) noexcept ->
                        std::optional<
                            typename MESSAGE_ID<S>::type>
                        {
                            if constexpr(
                                    !std::is_same_v<std::decay_t<decltype(message)>,std::monostate>)
                            {
                                auto def_enum_msg = []
                                    <MESSAGE_ID<S>::type T>(
                                    const Message<T>& msg) noexcept ->
                                    std::optional<typename MESSAGE_ID<S>::type>
                                {
                                    return std::optional<
                                        typename MESSAGE_ID<S>::type>(T);
                                };
                                return def_enum_msg(message);
                            }
                            else return std::nullopt;
                        };
                        return std::visit(msg_visitor,category);
                    }
                    else return std::nullopt;
                };
                return std::visit(cat_visitor,data_);
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

static_assert(std::is_default_constructible_v<network::MessageHandler<network::Side::CLIENT>>);
static_assert(std::is_default_constructible_v<network::MessageHandler<network::Side::SERVER>>);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::SERVER>>()>0);
static_assert(serialization::min_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);
static_assert(serialization::max_serial_size<network::MessageHandler<network::Side::CLIENT>>()>0);