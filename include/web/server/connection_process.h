#pragma once
#include "web/common/msgdef.h"
#include <string_view>
#include <memory>
#include <fstream>
#include <list>
#include <vector>

#include "network/abstractprocess.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "network/commonsocket.h"
#include "network/multiplexor/eventhandle.h"

namespace network{
    class ServerConnectionProcess:public AbstractConnectionProcess
    {
        MessageHandler<Side::CLIENT> recv_hmsg_;
        MessageHandler<Side::SERVER> send_hmsg_;

        void __task__(std::error_code& err, network::Client_MsgT::type msg_id){
            switch(msg_id){
                case Client_MsgT::SERVER_STATUS:
                    on_write(err);
                    return;
                case Client_MsgT::EXTRACT:
                    //heavy task
                    break;
            }
        }

        public:
        virtual void on_read(std::error_code& err) noexcept override{
            using namespace serialization;
            err.clear();
            io_context().receive(err,recv_hmsg_);
            if(err) {
                handle_receive_error(err);
                return;
            }
            if(auto msg_id = recv_hmsg_.message_type();
                msg_id.has_value())

                
            return;
        }
        virtual void on_write(std::error_code& err) noexcept override{
            if(!send_hmsg_.has_message()){
                err = std::make_error_code(std::errc::no_message);
                return;
            }
            return;
        }
        virtual void on_task_done(std::error_code& err) noexcept override{

        }
        virtual void on_stop_requested(std::error_code& err) noexcept override{

        }
        
        ServerConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
            AbstractConnectionProcess(hconn,err){}
        ~ServerConnectionProcess() = default;
        virtual void handle_event(
                    Event event,
                    std::error_code& err) noexcept
        {

        }
        virtual bool requestable() const noexcept override{
            return false;
        }
    };
}
