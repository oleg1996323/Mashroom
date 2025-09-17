#pragma once
#include <network/client.h>
#include <unordered_set>
#include "network/common/message/message_process.h"

namespace network{
    class ClientsHandler{
        private:
        mutable std::mutex mute_;
        using clients_t = std::vector<network::Client>;
        clients_t clients_;
        using clients_iterator = clients_t::iterator;
        clients_iterator __connect_internal__(const std::string& host,Port port);
        public:
        ErrorCode connect(const std::string& host,Port port);
        template<network::Client_MsgT::type MSG_T,typename... ARGS>
        ErrorCode request(bool wait,const std::string& host,const Port port,ARGS&&... args);
        template<network::Client_MsgT::type MSG_T,typename... ARGS>
        ErrorCode request(uint16_t timeout_sec,const std::string& host,const Port port,ARGS&&... args);
    };

    template<network::Client_MsgT::type MSG_T,typename... ARGS>
    ErrorCode ClientsHandler::request(bool wait, const std::string& host,Port port,ARGS&&... args){
        auto client_iter = __connect_internal__(host,port);
        if(client_iter==clients_.end())
            return ErrorCode::INTERNAL_ERROR;
        return client_iter->request<MSG_T>(wait,std::forward<ARGS>(args)...);
    }

    template<network::Client_MsgT::type MSG_T,typename... ARGS>
    ErrorCode ClientsHandler::request(uint16_t timeout_sec, const std::string& host,Port port,ARGS&&... args){
        auto client_iter = __connect_internal__(host,port);
        if(client_iter==clients_.end())
            return ErrorCode::INTERNAL_ERROR;
        return client_iter->request<MSG_T>(timeout_sec,std::forward<ARGS>(args)...);
    }
}