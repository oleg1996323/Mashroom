#include "web/client/connection_process.h"
#include "web/client.h"
#include "network/commonsocket.h"
#include "network/abstractprocess.h"
#include "sys/application.h"

namespace network{
    using namespace network;
    void ClientConnectionProcess::__reaction__(std::error_code& err, 
                network::Client_MsgT::type msg_id,
                Server_MsgT::type server_msg) noexcept{
        if(!version_ && msg_id!=Client_MsgT::ERROR && msg_id!=Client_MsgT::VERSION){
            __emplace_error__(err,
                "version interconnection not defined",
                ErrorCode::INVALID_CLIENT_REQUEST);
            return;
        }
        switch(msg_id){
            case Server_MsgT::CREDENTIALS:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::CREDENTIALS>();
                if(msg_ref.has_value()){
                    const auto& msg_credentials = msg_ref->get();
                    //@todo
                    if(!msg_credentials.success() && msg_credentials.attempts_left()>0){
                        std::cout<<"Incorrect login/password"<<std::endl;
                        std::string login;
                        
                        std::cout<<"login:";
                        std::cout.flush();
                        while(int ch = std::getchar()){
                            if(ch==3)
                                return;
                            else if(ch==13)
                                break;
                            else if(ch>31)
                                login.push_back(ch);
                            else continue;
                        }
                        std::cout<<"password:";
                        std::cout.flush();
                        std::string password;
                        while(int ch = std::getchar()){
                            if(ch==3)
                                return;
                            else if(ch==13)
                                break;
                            else if(ch>31)
                                password.push_back(ch);
                            else continue;
                        }
                        send_hmsg_.emplace_message(Message<Client_MsgT::CREDENTIALS>(login,password));
                    }
                }
                else{
                    __emplace_error__(err,
                                "credentials message handling",
                                ErrorCode::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::SERVER_STATUS:
            {
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::SERVER_STATUS>();
                if(msg_ref.has_value()){
                    auto& status = msg_ref->get();
                    if(status.status()==server::Status::INACTIVE)
                        io_context().enable_readable(false,err);
                        //add close connection to io_context
                }
                else{
                    __emplace_error__(err,
                                "server status message handling",
                                ErrorCode::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::TRANSACTION:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::TRANSACTION>();
                if(msg_ref.has_value()){
                    const auto& msg_transaction = msg_ref->get();
                    if(msg_transaction.state()==Transaction::DECLINE ||
                        msg_transaction.state()==Transaction::CANCEL){
                        file_recv_.reset();
                        waiting_.reset();
                    }
                    else if(msg_transaction.state()==Transaction::ACCEPT &&
                        file_recv_)
                    {
                        if(file_recv_->accepted())
                            file_recv_->next();
                        else
                            err = file_recv_->accept();
                    }
                    else err.clear();
                }
                else{
                    __emplace_error__(err,
                        "progress message handling",
                        ErrorCode::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::PROGRESS:
            {
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::PROGRESS>();
                if(msg_ref.has_value()){
                    const auto& msg_progress = msg_ref->get();
                    auto& transaction = msg_progress.transaction();
                    msg_progress.progress(); //@todo
                }
                else{
                    __emplace_error__(err,
                        "progress message handling",
                        ErrorCode::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::ERROR:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::ERROR>();
                if(msg_ref.has_value()){
                    if(msg_ref && msg_ref->get().transaction().has_value()){
                        auto& err_msg = msg_ref->get();
                        auto& transaction = msg_ref->get().transaction().value();
                        if(file_recv_ && file_recv_->meta().hash() == transaction.hash()){
                            if(err_msg.error()!=ErrorCode())
                                file_recv_.reset();
                        }
                    }
                    else{
                        //@todo
                    }
                }
                else{
                    __emplace_error__(err,
                        "error message handling",
                        ErrorCode::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::VERSION:
            {
                if(!version_.has_value()){
                    auto msg_ref = recv_hmsg_.get_message<Server_MsgT::VERSION>();
                    if(msg_ref.has_value()){
                        const auto& msg_version = msg_ref->get();
                        if(msg_version.version()>=
                            app().config().system_config().version())
                            __emplace_error__(err,"version error",ErrorCode::VERSION_ERROR_X1);
                        else version_ = msg_version.version();
                    }
                    else{
                        __emplace_error__(err,
                            "version message handling",
                            ErrorCode::INTERNAL_ERROR);
                    }
                }
                else{
                    __emplace_error__(err,
                        "version interconnection already defined",
                        ErrorCode::INVALID_CLIENT_REQUEST);
                }
            }
            break;
            case Server_MsgT::INDEX:{
                auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX>();
                if(msg_ref.has_value())
                    emplace_task<TaskMode::Thread>(err,
                        __index_process__,
                            ClientAppMsg(msg_ref->get()));
            }
            case Server_MsgT::FILE_METADATA:{

            }
            break;
            case Server_MsgT::FILE_DATA:{
                if(file_recv_){
                    auto msg_file_data = recv_hmsg_.get_message<Server_MsgT::FILE_DATA>();
                    if(msg_file_data.has_value()){
                        auto& file_data = msg_file_data->get();
                        if(file_data.transaction().hash()==file_recv_->meta().transaction().hash()){
                            file_recv_->next();
                        }
                        else  __emplace_error__(err,
                            "transaction "+file_data.hash()+" not found",
                            ErrorCode::RECEIVING_MESSAGE_ERROR);
                    }
                    else{
                        __emplace_error__(err,
                            "file data message handling",
                            ErrorCode::INTERNAL_ERROR);
                    }
                }
                else __emplace_error__(
                    err,
                    "unexpected message FILE_DATA",
                    ErrorCode::RECEIVING_MESSAGE_ERROR);
            }
            break;
        }
    }

    void ClientConnectionProcess::on_read(std::error_code& err) noexcept{
        using namespace serialization;
        err.clear();
        io_context().receive(err,recv_hmsg_);
        if(err) {
            handle_receive_error(err);
            return;
        }
        if(auto msg_id = recv_hmsg_.message_type();
            msg_id.has_value()){
            __task__(err,msg_id.value());            
        }
        return;
    }

    void ClientConnectionProcess::on_write(std::error_code& err) noexcept{
        if(!send_hmsg_.has_message()){
            err = std::make_error_code(std::errc::no_message);
            return;
        }
        else io_context().send(err,std::move(send_hmsg_));
        return;
    }

    void ClientConnectionProcess::on_task_done(std::error_code& err) noexcept{
        
    }
    void ClientConnectionProcess::on_stop_requested(std::error_code& err) noexcept{

    }
}