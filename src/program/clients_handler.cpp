#include "program/clients_handler.h"
#include <ranges>
#include <algorithm>

using namespace network;

ClientsHandler::clients_iterator ClientsHandler::__connect_internal__(const std::string& host,Port port){
    auto found = std::find_if(clients_.begin(),clients_.end(),[](const Client& client){
        return !client.is_connected();
    });
    if(found != clients_.end()){
        Client& client = *found;
        try{
            client.connect(host,port);
        }
        catch(const std::exception& err){
            ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            clients_.erase(found);
            return clients_.end();
        }
    }
    else{
        try{
            clients_.insert(clients_.end(),std::move(Client(host,port)))->connect();
        }
        catch(const std::exception& err){
            ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            assert(clients_.empty());
            clients_.pop_back();
            return clients_.end();
        }
    }
}

ErrorCode ClientsHandler::connect(const std::string& host,Port port){
    auto client = __connect_internal__(host,port);
    if(client==clients_.end())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client connection",AT_ERROR_ACTION::CONTINUE);
    return ErrorCode::NONE;
}