#pragma once
#include <network/client.h>
#include <unordered_set>
#include <network/client/message.h>

namespace network{
    class ClientsHandler{
        private:
        mutable std::mutex mute_;
        using clients_t = std::unordered_set<network::client::Client>;
        clients_t clients_;
        using clients_iterator = decltype(clients_)::iterator;
        clients_iterator __connect_internal__(const std::string& host);
        public:
        ErrorCode connect(const std::string& host);
        template<network::client::TYPE_MESSAGE MSG_T>
        ErrorCode request(const std::string& host,const network::client::Message<MSG_T>& msg);
        
    };

    template<network::client::TYPE_MESSAGE MSG_T>
    ErrorCode ClientsHandler::request(const std::string& host,const network::client::Message<MSG_T>& msg){
        auto client_iter = __connect_internal__(host);
        if(client_iter==clients_.end())
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client connection",AT_ERROR_ACTION::CONTINUE);
        if(client_iter->err_!=ErrorCode::NONE)
            return client_iter->err_;
        else return client_iter->request(msg);
    }
}