#include <network/server/connection_process.h>
#include <network/server.h>

using namespace connection;
using namespace server;
Process::Process(int connection_socket,const ConnectionPool& pool, size_t buffer_sz):

                    buffer_([buffer_sz]()->
                    std::vector<char>
                    {
                        std::vector<char> res;
                        res.resize(buffer_sz);
                        return res;
                    }()),
                    pool_(pool),
                    connection_socket_(connection_socket){}
Process::Process(Process&& other):pool_(other.pool_){
    if(this!=&other){
        buffer_.swap(other.buffer_);
        thread_.swap(other.thread_);
        std::swap(connection_socket_,other.connection_socket_);
    }
}
Process::~Process(){
    if(thread_.joinable())
        thread_.join();
    if(is_connected()){
        ::close(connection_socket_);
    }
}
void Process::set_buffer_size(size_t sz){
    buffer_.resize(sz);
}
void Process::__send_error_and_close_connection__(ErrorCode err, const char* message) const{
    server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
    fcntl(connection_socket_,F_SETFL,fcntl(connection_socket_, F_GETFL, 0)|SOCK_NONBLOCK);
    if(msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK)){
        struct pollfd pfd = {.fd=connection_socket_,.events=POLLOUT,.revents=0};
        int poll_return = poll(&pfd,1,1000);
        if(poll_return>0 && (pfd.revents&POLLOUT)){
            msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK);
        }
    }
    ::close(connection_socket_);
    return;
}
void Process::__send_error_and_continue__(ErrorCode err,const char* message) const{
    server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
    fcntl(connection_socket_,F_SETFL,fcntl(connection_socket_, F_GETFL, 0)|SOCK_NONBLOCK);
    if(msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK)){
        struct pollfd pfd = {.fd=connection_socket_,.events=POLLOUT,.revents=0};
        int poll_return = poll(&pfd,1,1000);
        if(poll_return>0 && (pfd.revents&POLLOUT)){
            msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK);
        }
    }
    fcntl(connection_socket_,F_SETFL,fcntl(connection_socket_, F_GETFL, 0)^SOCK_NONBLOCK);
    return;
}
ErrorCode Process::__read_rest_data_at_error__(client::TYPE_MESSAGE msg) const{
    size_t to_read=0;
    while (to_read<client::sizes_msg_struct[(int)msg]-1)
    {
        if(int readed = recv(this->connection_socket_,&buffer_,client::sizes_msg_struct[(int)msg-1],MSG_DONTWAIT)==-1){
            server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
            fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
            msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
            ::close(connection_socket_);
            return ErrorCode::INTERNAL_ERROR;
        }
        else to_read+=readed;
    }
    return ErrorCode::NONE;
}
bool Process::__check_and_notify_if_server_inaccessible__() const{
    if(pool_.server_status()!=Status::READY){
        __send_error_and_continue__(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"Server is currently inaccessible. Try later");
        return true;
    }
    else return false;
}
ErrorCode Process::execute() const{
    busy_ = true;
    bool heavy = false;
    client::TYPE_MESSAGE msg_t;
    if(recv(this->connection_socket_,&msg_t,sizeof(msg_t),MSG_DONTWAIT)==-1){
        server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
        fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
        msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
        ::close(connection_socket_);
        busy_=false;
        return ErrorCode::INTERNAL_ERROR;
    }
    switch(msg_t){
        case client::TYPE_MESSAGE::METEO_REQUEST:{
            if(__check_and_notify_if_server_inaccessible__()){
                ErrorCode err = __read_rest_data_at_error__(msg_t);
                busy_=false;
                if(err !=ErrorCode::NONE)
                    return err;
                else
                    return ErrorCode::INVALID_CLIENT_REQUEST;
            }
            heavy = true;
        }
        default:{
            __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Unknown request");
            return ErrorCode::INVALID_CLIENT_REQUEST;
        }
    }
    assert(busy_);
    if(heavy==true){
        ErrorCode err = __execute_heavy_process__(msg_t);
        busy_=false;
        return err;
    }
    else{
        ErrorCode err = __execute_light_process__(msg_t);
        busy_=false;
        return err;
    }
}
ErrorCode Process::__execute_heavy_process__(client::TYPE_MESSAGE msg_t) const{
    size_t to_read = 0;
    while (to_read<client::sizes_msg_struct[(int)msg_t]-1)
    {
        if(int readed = recv(this->connection_socket_,buffer_.data(),client::sizes_msg_struct[(int)msg_t],0)==-1){
            server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
            fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
            msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
            ::close(connection_socket_);
            return ErrorCode::INTERNAL_ERROR;
        }
        else to_read+=readed;
    }
    thread_ = std::move(std::jthread([this,msg_t](){
        ErrorCode err;
        switch(msg_t){
            case client::TYPE_MESSAGE::METEO_REQUEST:{
                client::Message<client::TYPE_MESSAGE::METEO_REQUEST> msg;
                Extract hExtract = msg.prepare_and_check_integrity_extractor(err);
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
                    server::Message<server::TYPE_MESSAGE::METEO_REPLY> reply_msg;
                    if(fs::is_regular_file(hExtract.out_path())){
                        translating_.wait(false);
                        if(!reply_msg.sendto(connection_socket_,hExtract.out_path())){
                            __send_error_and_continue__(ErrorCode::SENDING_REPLY_ERROR,"meteo request");
                            err = ErrorCode::SENDING_REPLY_ERROR;
                            translating_=false;
                            break;
                        }
                        translating_ = false;
                    }
                    else{
                        translating_.wait(false);
                        for(fs::directory_entry entry:fs::directory_iterator(hExtract.out_path()))
                            if(!reply_msg.sendto(connection_socket_,entry.path())){
                                __send_error_and_continue__(ErrorCode::SENDING_REPLY_ERROR,"meteo request");
                                err = ErrorCode::SENDING_REPLY_ERROR;
                                translating_=false;
                                break;
                            }
                        translating_=false;
                    }
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
ErrorCode Process::__execute_light_process__(client::TYPE_MESSAGE msg_t) const{
    switch(msg_t){
        default:{
            __send_error_and_continue__(ErrorCode::INVALID_CLIENT_REQUEST,"");
            return ErrorCode::INVALID_CLIENT_REQUEST;
        }
    }
}
ErrorCode Process::send_status_message(server::Status status) const{
    translating_ = true;
    server::Message<server::TYPE_MESSAGE::SERVER_CHECK> msg;
    msg.status_ = status;
    bool res = msg.sendto(connection_socket_);
    translating_ = false;
    if(!res)
        return ErrorPrint::print_error(ErrorCode::SENDING_REPLY_ERROR,"",AT_ERROR_ACTION::CONTINUE);
    else return ErrorCode::NONE;
}
int Process::socket() const{
    return connection_socket_;
}
bool Process::is_connected() const{
    int error = 0;
    socklen_t len = sizeof(error);
    int ret = getsockopt(connection_socket_,SOL_SOCKET,SO_ERROR,&error,&len);
    if(ret!=0 || error!=0)
        return false;
    return true;
}
void Process::shut() const{

}
void Process::shut_at_done() const{

}
void Process::close() const{
    
}
void Process::close_at_done() const{

}