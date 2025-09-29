#include "program/clients_handler.h"
#include <ranges>
#include <algorithm>

using namespace network;

ClientsHandler::clients_iterator ClientsHandler::__connect_internal__(const std::string& host,Port port){
    auto found = std::find_if(clients_.begin(),clients_.end(),[](const std::shared_ptr<RequestInstance>& client){
        return client && !client->connection_established();
    });
    if(found != clients_.end()){
        std::shared_ptr<RequestInstance> client = *found;
        try{
            client->__connect__(host,port);
            return found;
        }
        catch(const std::exception& err){
            ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            clients_.erase(found);
            return clients_.end();
        }
    }
    else{
        try{
            auto inserted = clients_.insert(clients_.end(),std::move(RequestInstance::__make_instance__(host,port)));
            (*inserted)->client_.connect();
            return inserted;
        }
        catch(const std::runtime_error& err){
            if(!clients_.empty());
                clients_.pop_back();
            ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            return clients_.end();
        }
        catch(const std::invalid_argument& err){
            if(!clients_.empty());
                clients_.pop_back();
            ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            return clients_.end();
        }
        catch(const std::exception& err){
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,err.what(),AT_ERROR_ACTION::ABORT);
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