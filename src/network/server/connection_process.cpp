#include "network/server/connection_process.h"
#include "network/server.h"
#include "proc/extract.h"
#include "proc/capitalize.h"

using namespace network::connection;
Process<Server>::Process(int connection_socket,const ConnectionPool& pool):
                    pool_(pool),
                    sock_(connection_socket){}
Process<Server>::Process(Process&& other):pool_(other.pool_){
    if(this!=&other){
        thread_.swap(other.thread_);
        std::swap(sock_,other.sock_);
    }
}
Process<Server>::~Process(){
    if(thread_.joinable())
        thread_.join();
    if(is_connected()){
        ::close(sock_);
    }
}
ErrorCode Process<Server>::__send_error_and_close_connection__(ErrorCode err, const char* message) const{
    Message<Server_MsgT::ERROR> msg_err;
    fcntl(sock_,F_SETFL,fcntl(sock_, F_GETFL, 0)|SOCK_NONBLOCK);
    if(err = mprocess_.send_message<Server_MsgT::ERROR>(sock_,ErrorCode::RECEIVING_MESSAGE_ERROR,pool_.server_status());err!=ErrorCode::NONE){
        close();
        return ErrorPrint::print_error(ErrorCode::CONNECTION_CLOSED,"",AT_ERROR_ACTION::CONTINUE);
    }
    close();
    return ErrorCode::NONE;
}
ErrorCode Process<Server>::__send_error_and_continue__(ErrorCode err,const char* message) const{
    fcntl(sock_,F_SETFL,fcntl(sock_, F_GETFL, 0)|SOCK_NONBLOCK);
    if(err = mprocess_.send_message<Server_MsgT::ERROR>(sock_,ErrorCode::RECEIVING_MESSAGE_ERROR,pool_.server_status());err!=ErrorCode::NONE){
        close();
        return ErrorPrint::print_error(ErrorCode::CONNECTION_CLOSED,"",AT_ERROR_ACTION::CONTINUE);
    }
    // if(msg_err.sendto(sock_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK)){
    //     struct pollfd pfd = {.fd=sock_,.events=POLLOUT,.revents=0};
    //     int poll_return = poll(&pfd,1,1000);
    //     if(poll_return>0 && (pfd.revents&POLLOUT)){
    //         msg_err.sendto(sock_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK);
    //     }
    // }
    fcntl(sock_,F_SETFL,fcntl(sock_, F_GETFL, 0)^SOCK_NONBLOCK);
    return ErrorCode::NONE;
}
bool Process<Server>::__check_and_notify_if_server_inaccessible__() const{
    if(pool_.server_status()!=server::Status::READY){
        __send_error_and_continue__(ErrorCode::RECEIVING_MESSAGE_ERROR,"Server is currently inaccessible. Try later");
        return true;
    }
    else return false;
}
ErrorCode Process<Server>::execute() const{
    busy_ = true;
    bool heavy = false;
    ErrorCode err_ = mprocess_.receive_any_message(sock_);
    auto msg_type = mprocess_.received_message_type();
    if(err_!=ErrorCode::NONE || !msg_type.has_value()){
        return __send_error_and_continue__(err_,"");
    }
    else{
        switch (msg_type.value())
        {
        case Client_MsgT::CAPITALIZE:
            
        break;
        case Client_MsgT::CAPITALIZE_REF:

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
    assert(busy_);
    if(heavy==true){
        ErrorCode err = __execute_heavy_process__(msg_type.value());
        busy_=false;
        return err;
    }
    else{
        ErrorCode err = __execute_light_process__(msg_type.value());
        busy_=false;
        return err;
    }
}
ErrorCode Process<Server>::__execute_heavy_process__(Client_MsgT::type msg_t) const{
    thread_ = std::move(std::jthread([this,msg_t](){
        ErrorCode err;
        switch(msg_t){
            case Client_MsgT::DATA_REQUEST:{
                Message<Client_MsgT::DATA_REQUEST> msg;
                Extract hExtract;
                //hExtract.set_by_request();
                if(err!=ErrorCode::NONE){
                    __send_error_and_continue__(err,"");
                    break;
                }
                else err = hExtract.execute(); //get output directory
                if(err!=ErrorCode::NONE){
                    __send_error_and_continue__(err,"");
                    break;
                }
                if(!fs::is_directory(hExtract.out_path()) && !fs::is_regular_file(hExtract.out_path())){ //must be directory|regular file (tmp (temporary) in case of internet transaction)
                    __send_error_and_close_connection__(ErrorCode::INTERNAL_ERROR,"Something went wrong (server side)");
                    err = ErrorCode::INTERNAL_ERROR;
                    break;
                }
                else {
                    if(fs::is_regular_file(hExtract.out_path())){
                        if(fs::file_size(hExtract.out_path())==0)
                            return __send_error_and_continue__(ErrorCode::DATA_NOT_FOUND,"");
                        else{
                            if(err = mprocess_.send_message<Server_MsgT::DATA_REPLY_FILEINFO>(sock_,pool_.server_status(),hExtract.out_path(),0,fs::file_size(hExtract.out_path())); err!=ErrorCode::NONE)
                                __send_error_and_continue__(ErrorCode::SENDING_MESSAGE_ERROR,"");
                            return err;
                            if(err = mprocess_.receive_message<Client_MsgT::TRANSACTION>(sock_);err!=ErrorCode::NONE){
                                __send_error_and_continue__(ErrorCode::RECEIVING_MESSAGE_ERROR,"");
                                return err;
                            }
                            auto& msg = mprocess_.get_received_message<Client_MsgT::TRANSACTION>().value().get();
                            if(msg.additional().op_status_!=Transaction::ACCEPT)
                                return ErrorPrint::print_error(ErrorCode::TRANSACTION_REFUSED,"",AT_ERROR_ACTION::CONTINUE);
                            else return ErrorCode::NONE;
                        }
                    }
                    else{
                        __send_error_and_continue__(ErrorCode::SERVER_ERROR,"");
                        return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_FILE,"",AT_ERROR_ACTION::CONTINUE,hExtract.out_path().string());
                    }
                    // else{
                    //     translating_.wait(false);
                    //     for(fs::directory_entry entry:fs::directory_iterator(hExtract.out_path()))
                    //         if(!reply_msg.sendto(sock_,entry.path())){
                    //             __send_error_and_continue__(ErrorCode::SENDING_MESSAGE_ERROR,"meteo request");
                    //             err = ErrorCode::SENDING_MESSAGE_ERROR;
                    //             translating_=false;
                    //             break;
                    //         }
                    //     translating_=false;
                    // }
                }
                break;
            }
            default:{
                __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Unknown request");
                err = ErrorCode::INVALID_CLIENT_REQUEST;
                break;
            }
        }
        return err;
    }));
    return ErrorCode::NONE;
}
ErrorCode Process<Server>::__execute_light_process__(Client_MsgT::type msg_t) const{
    switch(msg_t){
        default:{
            __send_error_and_continue__(ErrorCode::INVALID_CLIENT_REQUEST,"");
            return ErrorCode::INVALID_CLIENT_REQUEST;
        }
    }
}
ErrorCode Process<Server>::send_status_message(server::Status status) const{
    translating_ = true;
    if(ErrorCode err = mprocess_.send_message<Server_MsgT::SERVER_STATUS>(sock_,pool_.server_status());err!=ErrorCode::NONE){
        __send_error_and_continue__(ErrorCode::SERVER_ERROR,"");
        return err;
    }
    translating_ = false;
    return ErrorCode::NONE;
}
int Process<Server>::socket() const{
    return sock_;
}
bool Process<Server>::is_connected() const{
    int error = 0;
    socklen_t len = sizeof(error);
    int ret = getsockopt(sock_,SOL_SOCKET,SO_ERROR,&error,&len);
    if(ret!=0 || error!=0)
        return false;
    return true;
}
void Process<Server>::shut() const{

}
void Process<Server>::shut_at_done() const{

}
void Process<Server>::close() const{
    
}
void Process<Server>::close_at_done() const{

}