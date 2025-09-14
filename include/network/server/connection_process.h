#pragma once
#include <network/common/def.h>
#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include "network/common/message/message_process.h"
#include <queue>
#include <shared_mutex>
#include "network/common/connection_process.h"

namespace network{
    class Server;
}
using namespace network;

namespace network::connection{
    class ConnectionPool;
    template<>
    class Process<Server>:public AbstractProcess<Process<Server>>{
        friend class ConnectionPool;
        private:
        ErrorCode err_;
        mutable MessageProcess<Side::SERVER> mprocess_;
        mutable std::atomic<bool> translating_ = false; //1 byte
        Process(const Process&) = delete;
        Process& operator=(Process&& other) = delete;   //thread_ may be launched and 
                                                        //containing old "other" pointer
        Process& operator=(const Process& other) = delete;
        ErrorCode __send_error_and_close_connection__(ErrorCode err,const char* msg_err) const;
        ErrorCode __send_error_and_continue__(ErrorCode err,const char* msg_err) const;
        bool __check_and_notify_if_server_inaccessible__() const;
        ErrorCode __execute_heavy_process__(network::Client_MsgT::type msg_t) const; //
        ErrorCode __execute_light_process__(network::Client_MsgT::type msg_t) const; //asyncronously
        virtual void execute(const std::stop_token& stop, const Socket& socket) override{
            bool heavy = false;
            err_ = mprocess_.receive_any_message(socket);
            auto msg_type = mprocess_.received_message_type();
            if(err_!=ErrorCode::NONE || !msg_type.has_value()){
                __send_error_and_continue__(err_,"");
            }
            else{
                switch (msg_type.value())
                {
                case Client_MsgT::INDEX:
                    
                break;
                case Client_MsgT::INDEX_REF:

                break;
                case Client_MsgT::DATA_REQUEST:
                            
                break;
                case Client_MsgT::SERVER_STATUS:

                break;
                case Client_MsgT::TRANSACTION:
                    
                break;
                default:
                    break;
                }
            }
            assert(busy());
            if(heavy==true)
                err_ = __execute_heavy_process__(msg_type.value());
            else
                err_ = __execute_light_process__(msg_type.value());
        }
        public:
        Process(Socket sock,const ConnectionPool& pool);
        Process(Process&& other);
        ~Process();
        void collapse() const;
        void collapse_at_ready() const;
        void close() const;
        void close_at_done() const;
    };
}