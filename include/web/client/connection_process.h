#pragma once
#include "web/common/msgdef.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "network/abstractprocess.h"
#include "network/worker/command.h"

namespace network{
    class ClientConnectionProcess:public AbstractRequestableConnectionProcess{

        void __reaction__(
                std::error_code& err,
                Client_MsgT::type client_msg,
                Server_MsgT::type server_msg) noexcept{
            switch(client_msg){
                case Client_MsgT::SERVER_STATUS:
                    if(server_msg!=Server_MsgT::SERVER_STATUS){
                        err=std::make_error_code(std::errc::bad_message);
                        return;
                    }
                    else {
                        err.clear();
                        complete_current_request(err);
                        make_active_request();
                        return;
                    }
                    break;
                case Client_MsgT::INDEX_REF:
                    if(server_msg!=Server_MsgT::INDEX){
                        err=std::make_error_code(std::errc::bad_message);
                        return;
                    }
                    else {
                        err.clear();
                        complete_current_request(err);
                        make_active_request();
                        return;
                    }
                    break;
                case Client_MsgT::INDEX:
                    if(server_msg==Server_MsgT::INDEX){
                        
                    }
                    else if(server_msg==Server_MsgT::FILE_METADATA){
                        
                    }
                    else if(server_msg==Server_MsgT::FILE_DATA){

                    }
                    else{
                        
                        err=std::make_error_code(std::errc::bad_message);
                        return;
                    }
                    break;

            }
        }

        public:
        virtual ~ClientConnectionProcess() = default;
        
        virtual void on_read(std::error_code& err) noexcept override{
            if(!active_request(err) && make_active_request())
                return;
            else{
                auto recv_res = io_context().receive(err,*active_request_->received());
                if(recv_res==-1){
                    handle_receive_error(err);
                    return;
                }
                else{
                    using send = Frame<std::monostate,MessageHandler<Side::CLIENT>,std::monostate>;
                    using recv = Frame<std::monostate,MessageHandler<Side::SERVER>,std::monostate>;
                    send* sent_;
                    recv* recv_;
                    if(auto msg_t = active_request_->received(recv_)->data_frame().message_type();
                        msg_t.has_value())
                    {
                        active_request_->received(sent_)->data_frame().message_type();
                    }

                        
                    io_context();
                    active_request_->bytes_received(recv_res);
                    if(active_request_->all_received(true)){
                        make_active_request();
                        return;
                    }
                    else return;
                }
            }
        }

        ClientConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
        AbstractRequestableConnectionProcess(hconn,err){}
        virtual void on_bad_send(std::error_code& err) noexcept{

        }
        virtual void on_bad_receive(std::error_code& err) noexcept{

        }
        virtual void on_task_done(std::error_code& err) noexcept override{

        }
        virtual void on_stop_requested(std::error_code& err) noexcept override{

        }
    };
}