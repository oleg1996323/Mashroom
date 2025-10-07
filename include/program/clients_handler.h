#pragma once
#include <network/client.h>
#include "network/common/message/message_process.h"
#include <list>

namespace network{
    class ClientsHandler;
    class RequestInstance:public std::enable_shared_from_this<RequestInstance>{
        friend class ClientsHandler;
        network::Client client_;
        RequestInstance& __connect__(const std::string& host,Port port){
            client_.connect(host,port);
            return *this;
        }
        static std::shared_ptr<RequestInstance> __make_instance__(const std::string& host, uint16_t port){
            return std::make_shared<RequestInstance>(host,port);
        } 
        public:
        RequestInstance(const std::string& host, uint16_t port):client_(host,port){}
        ~RequestInstance(){
            std::cout<<"Instance destroyed"<<std::endl;
        }
        template<network::Server_MsgT::type MSG>
        const auto& get_result(int16_t timeout_s) const{
            return client_.get_result<MSG>(timeout_s);
        }
        std::shared_ptr<RequestInstance> get_this(){
            return shared_from_this();
        }
        std::shared_ptr<const RequestInstance> get_this() const{
            return shared_from_this();
        }
        bool connection_established() const{
            return client_.has_socket();
        }
        template<network::Client_MsgT::type T,typename... ARGS>
        void request(bool wait,ARGS&&... args){
            client_.request<T>(wait,std::forward<ARGS>(args)...);
        }
        template<network::Client_MsgT::type T,typename... ARGS>
        void request(int timeout_sec,ARGS&&... args){
            client_.request<T>(timeout_sec,std::forward<ARGS>(args)...);
        }
    };

    class ClientsHandler{
        private:
        mutable std::mutex mute_;
        using clients_t = std::list<std::shared_ptr<RequestInstance>>;
        clients_t clients_;
        using clients_iterator = clients_t::iterator;
        clients_iterator __connect_internal__(const std::string& host,Port port);
        public:
        ErrorCode connect(const std::string& host,Port port);
        template<network::Client_MsgT::type MSG_T,typename... ARGS>
        std::shared_ptr<RequestInstance> request(bool wait,const std::string& host,const Port port,ARGS&&... args);
        template<network::Client_MsgT::type MSG_T,typename... ARGS>
        std::shared_ptr<RequestInstance> request(uint16_t timeout_sec,const std::string& host,const Port port,ARGS&&... args);
    };

    template<network::Client_MsgT::type MSG_T,typename... ARGS>
    std::shared_ptr<RequestInstance> ClientsHandler::request(bool wait, const std::string& host,Port port,ARGS&&... args){
        auto client_iter = __connect_internal__(host,port);
        if(client_iter==clients_.end())
            return std::shared_ptr<RequestInstance>();
        (*client_iter)->request<MSG_T>(wait,std::forward<ARGS>(args)...);
        return *client_iter;
    }

    template<network::Client_MsgT::type MSG_T,typename... ARGS>
    std::shared_ptr<RequestInstance> ClientsHandler::request(uint16_t timeout_sec, const std::string& host,Port port,ARGS&&... args){
        auto client_iter = __connect_internal__(host,port);
        if(client_iter==clients_.end())
            return std::shared_ptr<RequestInstance>();
        (*client_iter)->request<MSG_T>(timeout_sec,std::forward<ARGS>(args)...);
        return *client_iter;
    }
}