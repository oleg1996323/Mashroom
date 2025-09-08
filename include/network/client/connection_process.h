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
        private:
        Socket __socket__() const noexcept;
        public:
        friend class network::Client;
        mutable MessageProcess<Side::CLIENT> mprocess_;
        const Client* client_;
        Process(const Client* client):client_(client){}
        Process(const Process&) = delete;
        Process(Process&& other) noexcept;
        Process& operator=(const Process&) = delete;
        Process& operator=(Process&& other) noexcept;
        void swap(Process& other) noexcept;
        template<auto MSG_T,typename... ARGS>
        requires ClientMessageEnumConcept<MSG_T>
        ErrorCode send(ARGS&&... args) const{
            return mprocess_.send_message<MSG_T>(__socket__(),std::forward<ARGS>(args)...);
        }
        
        ErrorCode receive_any() const{
            return mprocess_.receive_any_message(__socket__());
        }

        template<auto MSG>
        requires ServerMessageEnumConcept<MSG>
        std::expected<std::reference_wrapper<Message<MSG>>,ErrorCode> receive_expected() const{
            return mprocess_.receive_message<MSG>(__socket__());
        }
    };
}