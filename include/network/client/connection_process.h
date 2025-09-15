#pragma once
#include <network/common/def.h>
#include "network/common/message/message_process.h"
#include "network/common/connection_process.h"
#include "abstractprocess.h"

namespace network{
    class Client;
}

using namespace network;

namespace network::connection{
    template<>
    class Process<Client>:public AbstractProcess<Process<Client>>{
        MessageProcess<Side::CLIENT> mprocess_;
        public:
        Process() = default;
        Process(const Process&) = delete;
        Process(Process&& other) noexcept;
        Process& operator=(const Process&) = delete;
        Process& operator=(Process&& other) noexcept;


        template<auto MSG_T,typename... ARGS>
        requires ClientMessageEnumConcept<MSG_T>
        ErrorCode send(const Socket& socket,ARGS&&... args){
            return mprocess_.send_message<MSG_T>(socket,std::forward<ARGS>(args)...);
        }
        ErrorCode receive_any(const Socket& socket){
            return mprocess_.receive_any_message(socket);
        }
        template<auto MSG>
        requires ServerMessageEnumConcept<MSG>
        std::expected<std::reference_wrapper<Message<MSG>>,ErrorCode> receive_expected(const Socket& socket){
            return mprocess_.receive_message<MSG>(socket);
        }
    };
}

static_assert(std::is_move_constructible_v<std::monostate>);
static_assert(std::is_move_assignable_v<std::monostate>);