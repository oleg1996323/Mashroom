#pragma once
#include <network/common/message/msgdef.h>
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"
#include <functional/def.h>

namespace network::client{
    namespace detail{
        class MessageHandler;
    }

    template<>
    struct AssociatedData<TYPE_MESSAGE::TRANSACTION>{
        ErrorCode err_ = ErrorCode::NONE;
    };

    template<>
    class network::client::Message<TYPE_MESSAGE::TRANSACTION>:public __Message__<TYPE_MESSAGE::TRANSACTION>{
        public:
        enum class Status{
            ACCEPT,
            DECLINE,
            CANCEL
        };
        private:
        /// @brief operation hash
        size_t op_hash_ = 0;
        Status op_status_ = Status::DECLINE;

        //const size_t current_hash_ = 0; //TODO: std::hash<fs::path> is temporary solution
        friend class ::network::client::detail::MessageHandler;
        public:

        using associated_t = AssociatedData<type_msg_>;
        private:
        Message(associated_t& a_t,size_t op_hash,Status op_status);

        Message(__Message__&& other_base);
        ErrorCode serialize_impl() const;
        ErrorCode deserialize_impl();
        public:
        size_t hash() const;
    };
}