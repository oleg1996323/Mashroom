#include "web/client/connection_process.h"
#include "web/client.h"
#include "OsterLib/network/commonsocket.h"
#include "OsterLib/network/abstractprocess.h"
#include "sys/application.h"
#include "OsterLib/parsing.h"
#include <boost/units/systems/information.hpp>
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/quantity.hpp>
#include "program/mashroom.h"

namespace network{
    using namespace network;
    ClientConnectionProcess::ClientConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
        AbstractRequestableConnectionProcess(hconn,err){}

    void ClientConnectionProcess::on_init_connection(std::error_code& err) noexcept{
        err.clear();
        send_hmsg_.emplace_message<Client_MsgT::VERSION>().version(
            app().config().system_config().version());
        push_request(
            std::make_shared<RequestCommandSpec<MessageHandler<Side::SERVER>,
                std::monostate,
                MessageHandler<Side::CLIENT>,
                std::monostate>>(
                    connection_handle(),
                    std::monostate(),
                    std::move(send_hmsg_),
                    std::monostate()),err);
        assert(!send_hmsg_.has_message());
    }
    void ClientConnectionProcess::__reaction__(std::error_code& err,
                Server_MsgT::type msg_id) noexcept{
        if(!version_ && msg_id!=Server_MsgT::VERSION && msg_id!=Server_MsgT::ERROR){
            __emplace_error__(err,
                "version interconnection not defined",
                mashroom::errc::INVALID_CLIENT_REQUEST);
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
                                mashroom::errc::INTERNAL_ERROR);
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
                                mashroom::errc::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::TRANSACTION:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::TRANSACTION>();
                if(msg_ref.has_value()){
                    const auto& msg_transaction = msg_ref->get();
                    if((msg_transaction.state()==Transaction::DECLINE ||
                        msg_transaction.state()==Transaction::CANCEL) &&
                        file_recv_.contains(msg_transaction.hash())){
                        file_recv_.erase(msg_transaction.hash());
                    }
                    else if(msg_transaction.state()==Transaction::ACCEPT)
                    {
                        if(file_recv_.contains(msg_transaction.hash()))
                            break;
                        else __emplace_error__(err,
                        "transaction "+msg_transaction.hash()+" not found",
                        mashroom::errc::RECEIVING_MESSAGE_ERROR);
                    }
                    else err.clear();
                }
                else{
                    __emplace_error__(err,
                        "progress message handling",
                        mashroom::errc::INTERNAL_ERROR);
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
                        mashroom::errc::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::ERROR:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::ERROR>();
                if(msg_ref.has_value()){
                    if(msg_ref && msg_ref->get().transaction().has_value()){
                        auto& err_msg = msg_ref->get();
                        auto& transaction = msg_ref->get().transaction().value();
                        if(file_recv_.contains(transaction.hash())){
                            if(err_msg.error()!=mashroom::errc())
                                file_recv_.erase(transaction.hash());
                        }
                    }
                    else{
                        //@todo
                    }
                }
                else{
                    __emplace_error__(err,
                        "error message handling",
                        mashroom::errc::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::VERSION:
            {
                if(!version_.has_value()){
                    auto msg_ref = recv_hmsg_.get_message<Server_MsgT::VERSION>();
                    if(msg_ref.has_value()){
                        const auto& msg_version = msg_ref->get();
                        if(msg_version.version()>
                            app().config().system_config().version())
                            __emplace_error__(err,"version error",mashroom::errc::VERSION_ERROR_X1);
                        else version_ = msg_version.version();
                        if(is_active_request())
                            complete_current_request(err);
                        assert(!is_active_request());
                    }
                    else{
                        __emplace_error__(err,
                            "version message handling",
                            mashroom::errc::INTERNAL_ERROR);
                    }
                }
                else{
                    __emplace_error__(err,
                        "version interconnection already defined",
                        mashroom::errc::INVALID_CLIENT_REQUEST);
                }
            }
            break;
            case Server_MsgT::INDEX:{
                auto msg_ref = recv_hmsg_.get_message<Server_MsgT::INDEX>();
                if(msg_ref.has_value()){
                    for(auto& block:msg_ref->get().index_blocks()){
                        auto in_block = [](auto& val){
                            if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(val)>>)
                                return;
                            else Mashroom::instance().data().update_indexing(val);
                        };
                    }
                    err.clear();
                    if(is_active_request()){
                        recv_t* msg_ptr_t;
                        auto recv = active_request_->received(msg_ptr_t);
                        assert(recv);
                        recv->data_frame()=std::move(recv_hmsg_);
                    }
                    complete_current_request(err);
                }
                else{
                    __emplace_error__(err,
                        "index message handling",
                        mashroom::errc::INTERNAL_ERROR);
                    complete_current_request(err);
                }
            }
            break;
            case Server_MsgT::FILE_METADATA:{
                auto msg_meta = recv_hmsg_.get_message<Server_MsgT::FILE_METADATA>();
                if(msg_meta.has_value()){
                    auto& file_data = msg_meta->get();
                    if(!file_recv_.contains(file_data.transaction().hash()) ||
                        !file_recv_.at(file_data.transaction().hash()).canceled()){
                            boost::units::information::bytes*file_data.file_size();
                        uint16_t info_digit = uint16_t(std::log(file_data.file_size())/std::log(2))/10;
                        uint16_t integer_val = file_data.file_size()/size_t(std::pow(1024,info_digit));
                        uint16_t comma_val = info_digit>0?(file_data.file_size()-size_t(integer_val)*size_t(std::pow(1024,info_digit)))/
                            size_t(std::pow(1024,info_digit-1)):0;
                        std::cout<<"File:"<<file_data.filename()<<";size:"<<integer_val;
                        if(comma_val!=0)
                            std::cout<<"."<<comma_val;
                        switch(info_digit){
                            case 0:
                                std::cout<<"B";
                            break;
                            case 1:
                                std::cout<<"KB";
                            break;
                            case 2:
                                std::cout<<"MB";
                            break;
                            case 3:
                                std::cout<<"GB";
                            break;
                            case 4:
                                std::cout<<"TB";
                            break;
                            case 5:
                                std::cout<<"PB";
                            break;
                            default:
                                __emplace_error__(err,
                                "file metadata message handling",
                                mashroom::errc::INTERNAL_ERROR);
                            break;
                        }
                        std::cout<<std::endl;
                        std::cout<<"Accept? (YES/no):"<<std::endl;
                        std::string input;
                        while(true){
                            std::getline(std::cin,input);
                            if(std::equal(  input.begin(),
                                            input.end(),
                                            "yes",
                                            case_insensitive_char_compare) ||
                                        input.empty()){
                                file_recv_.insert({file_data.transaction().hash(),ReceivingFileState(msg_meta->get())});
                                Message<Client_MsgT::TRANSACTION> accept_msg;
                                accept_msg.state(Transaction::ACCEPT);
                                send_hmsg_.emplace_message(accept_msg);
                            }
                            else if(std::equal(  input.begin(),
                                            input.end(),
                                            "no",
                                            case_insensitive_char_compare)){
                                Message<Client_MsgT::TRANSACTION> cancel_msg;
                                cancel_msg.state(Transaction::CANCEL);
                                send_hmsg_.emplace_message(cancel_msg);
                            }
                            else continue;
                        }
                        
                    }
                    else  __emplace_error__(err,
                        "transaction "+file_data.hash()+" already in process",
                        mashroom::errc::RECEIVING_MESSAGE_ERROR);
                }
                else{
                    __emplace_error__(err,
                        "file metadata message handling",
                        mashroom::errc::INTERNAL_ERROR);
                }
            }
            break;
            case Server_MsgT::FILE_DATA:{
                auto msg_file_data = recv_hmsg_.get_message<Server_MsgT::FILE_DATA>();
                if(msg_file_data.has_value()){
                    auto& file_data = msg_file_data->get();
                    if(file_recv_.contains(file_data.transaction().hash()) &&
                        !file_recv_.at(file_data.transaction().hash()).canceled())
                    {
                        file_recv_.at(file_data.transaction().hash()).next(file_data);
                    }
                    else  __emplace_error__(err,
                        "transaction "+file_data.hash()+" not found",
                        mashroom::errc::RECEIVING_MESSAGE_ERROR);
                }
                else{
                    __emplace_error__(err,
                        "file data message handling",
                        mashroom::errc::INTERNAL_ERROR);
                }
            }
            break;
        }

    }

    void ClientConnectionProcess::on_read(std::error_code& err) noexcept{
            auto recv_res = io_context().receive(err,recv_hmsg_);
            if(err){
                if(!handle_receive_error(err))
                    recv_hmsg_.clear();
                return;
            }
            else{
                if(auto msg_id = recv_hmsg_.message_type();
                    msg_id.has_value()){
                    __reaction__(err,*msg_id);
                    recv_hmsg_.clear();
                    on_write(err);
                }
                else {
                    __emplace_error__(err,"bad message received",mashroom::errc::RECEIVING_MESSAGE_ERROR);
                    if(is_active_request()){
                        complete_current_request(std::make_error_code(std::errc::bad_message));
                        on_write(err);
                    }
                }
            }
        }

    void ClientConnectionProcess::on_write(std::error_code& err) noexcept{
        auto sent = io_context().send_rest(err);
        if(!handle_sending_error(err))
            return;
        if(send_hmsg_.has_message()){
            std::cout<<"client hmsg index to send: "<<*send_hmsg_.message_type()<<std::endl;
            io_context().send(err,
                    [](const std::vector<char>&){},
                    std::move(send_hmsg_));
            send_hmsg_.clear();
            handle_sending_error(err);
        }
        else{
            if(!next_request())
                return;
            else{
                using send_t = Frame<std::monostate,MessageHandler<Side::CLIENT>,std::monostate>;
                send_t* s;
                send_hmsg_ = active_request_->sent(s)->data_frame();
                assert(send_hmsg_.has_message() && send_hmsg_.message_type().has_value());
                std::cout<<"client hmsg index to send: "<<*send_hmsg_.message_type()<<std::endl;
                assert(send_hmsg_.has_message());
                io_context().send(err,
                    [](const std::vector<char>&){},
                    std::move(send_hmsg_));
                send_hmsg_.clear();
                assert(!send_hmsg_.has_message());
                handle_sending_error(err);
                return;
            }
        }
        return;
    }

    void ClientConnectionProcess::on_task_done(std::error_code& err) noexcept{
        
    }
    void ClientConnectionProcess::on_stop_requested(std::error_code& err) noexcept{

    }
}