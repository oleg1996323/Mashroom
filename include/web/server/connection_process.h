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
        public:
        virtual void on_read(std::error_code& err) noexcept override{
            using namespace serialization;
            auto at_error = [this](std::errc c){
                recv_hmsg_ = std::move(recv_hmsg_);
                return std::make_error_code(c);
            };
            io_context().receive(err,io_context().free_space());
            if(err!=std::error_code())
                return;
            else {
                if(auto ser_res = io_context().deserialize(recv_hmsg_);ser_res!=serialization::SerializationEC::NONE)
                {
                    if(ser_res==serialization::SerializationEC::BUFFER_SIZE_LESSER)
                        err = std::make_error_code(std::errc::resource_unavailable_try_again);
                    else{
                        err = std::make_error_code(std::errc::bad_message);
                        MessageHandler<Side::SERVER> err_msg;
                        err_msg.emplace_message_by_id(
                            Message_t<Side::SERVER>::ERROR,
                            ErrorCode::RECEIVING_MESSAGE_ERROR,
                            server::Status::READY);
                        io_context().serialize(err_msg);
                        io_context().send(err);
                    }
                    return;
                }
            }
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
