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
        clients_iterator __connect_internal__(const std::string& host,const std::string& port);
        public:
        ErrorCode connect(const std::string& host,const std::string& port);
        template<network::Client_MsgT::type MSG_T,typename... ARGS>
        ErrorCode request(const std::string& host,const std::string& port,ARGS&&... args);
        
    };

    template<network::Client_MsgT::type MSG_T,typename... ARGS>
    ErrorCode ClientsHandler::request(const std::string& host,const std::string& port,ARGS&&... args){
        auto client_iter = __connect_internal__(host,port);
        if(client_iter==clients_.end())
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client connection",AT_ERROR_ACTION::CONTINUE);
        if(client_iter->err_!=ErrorCode::NONE)
            return client_iter->err_;
        else return client_iter->request<MSG_T>(std::forward<ARGS>(args)...);
    }
}