#pragma once
#include "web/common/msgdef.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "network/abstractprocess.h"

namespace network{
    class ClientConnectionProcess:public AbstractRequestableConnectionProcess{
        public:
        virtual ~ClientConnectionProcess() = default;    
        ClientConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
        AbstractRequestableConnectionProcess(hconn,err){}
        virtual void on_bad_serialization(
                serialization::SerializationEC ser_c,
                std::error_code& err) noexcept
        {

        }
        virtual void on_bad_deserialization(
                serialization::SerializationEC ser_c,
                std::error_code& err)
        {

        }
        virtual void on_bad_send(std::error_code& err) noexcept{

        }
        virtual void on_bad_receive(std::error_code& err) noexcept{

        }
        virtual void on_read(std::error_code& err) noexcept override{
            std::cout<<"Client: receive message"<<std::endl;
            try_receive(err);
            if(err!=std::error_code())
            {   
                switch(static_cast<std::errc>(err.value())){
                    case std::errc::resource_unavailable_try_again:
                    case std::errc::operation_in_progress:
                    case std::errc::no_buffer_space:
                        std::cout<<"(client) on read error: "<<err.message()<<std::endl;
                        err.clear();
                        return;
                    default:
                        complete_current_request(err);
                        if(make_active_request())
                            on_write(err);
                        else return;
                }
            }
            else{
                complete_current_request(err);
                if(make_active_request())
                    on_write(err);
                else return;
            }
        }
        virtual void on_write(std::error_code& err) noexcept override{
            std::cout<<"Client: sending message"<<std::endl;
            bool all_sent = try_send(err);
            if(err!=std::error_code()){
                switch(static_cast<std::errc>(err.value())){
                    case std::errc::resource_unavailable_try_again:
                    case std::errc::operation_in_progress:
                    case std::errc::no_buffer_space:
                        std::cout<<"(client) on write error: "<<err.message()<<std::endl;
                        err.clear();
                        return;
                    default:
                        std::cout<<err.message()<<std::endl;
                        complete_current_request(err);
                        if(make_active_request())
                            on_write(err);
                        else return;
                }
            }
            else err.clear();
        }
        virtual void on_task_done(std::error_code& err) noexcept override{

        }
        virtual void on_stop_requested(std::error_code& err) noexcept override{

        }
    };
}