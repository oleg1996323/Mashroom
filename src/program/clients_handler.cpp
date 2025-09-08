#include "program/clients_handler.h"
#include <ranges>
#include <algorithm>

using namespace network;

ClientsHandler::clients_iterator ClientsHandler::__connect_internal__(const std::string& host,const std::string& port){
    network::Client client(host,port);
    if(client.err_!=ErrorCode::NONE)
        return clients_.end();
    if(auto found = std::find(clients_.begin(),clients_.end(),client);found==clients_.end()){
        if(client.connect(host,port)!=ErrorCode::NONE)
            return clients_.end();
        return clients_.insert(clients_.end(),std::move(client));
    }
    else{
        if(found->connect(host,port)!=ErrorCode::NONE)
            return found;
        else {
            clients_.erase(found);
            return clients_.end();
        }
    }
}

ErrorCode ClientsHandler::connect(const std::string& host,const std::string& port){
    auto client = __connect_internal__(host,port);
    if(client==clients_.end())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client connection",AT_ERROR_ACTION::CONTINUE);
    if(client->err_!=ErrorCode::NONE)
        return client->err_;
    return ErrorCode::NONE;
}