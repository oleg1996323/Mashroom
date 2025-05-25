#include <program/clients_handler.h>

using namespace network;

ClientsHandler::clients_iterator ClientsHandler::__connect_internal__(const std::string& host){
    network::client::Client client(host);
    if(client.err_!=ErrorCode::NONE)
        return clients_.end();
    if(!clients_.contains(client)){
        if(client.connect()!=ErrorCode::NONE)
            return clients_.end();
        return clients_.emplace(std::move(client)).first;
    }
    else return clients_.find(client);
}

ErrorCode ClientsHandler::connect(const std::string& host){
    auto client = __connect_internal__(host);
    if(client==clients_.end())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client connection",AT_ERROR_ACTION::CONTINUE);
    if(client->err_!=ErrorCode::NONE)
        return client->err_;
    return ErrorCode::NONE;
}