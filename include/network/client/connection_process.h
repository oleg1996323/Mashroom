#pragma once
#include <network/common/def.h>
#include "network/common/message/message_process.h"
#include "network/common/connection_process.h"

namespace network{
    class Client;
}

using namespace network;

namespace network::connection{
    template<>
    class Process<Client>{
        friend class network::Client;
        mutable MessageProcess<Side::CLIENT> mprocess_;
        Socket sock_;
        Process() = default;
        Process(const Process&) = delete;
        Process& operator=(const Process&) = delete;
        Process(Process&& other) noexcept;
        void swap(Process& other) noexcept;
        Process& operator=(Process&& other) noexcept;
        template<auto MSG_T,typename... ARGS>
        requires ClientMessageEnumConcept<MSG_T>
        ErrorCode send(ARGS&&... args) const{
            return mprocess_.send_message<MSG_T>(sock_,std::forward<ARGS>(args)...);
        }
        
        ErrorCode receive_any() const{
            return mprocess_.receive_any_message(sock_);
        }

        template<auto MSG>
        requires ServerMessageEnumConcept<MSG>
        std::expected<std::reference_wrapper<Message<MSG>>,ErrorCode> receive_expected() const{
            return mprocess_.receive_message<MSG>(sock_);
        }
    };
}