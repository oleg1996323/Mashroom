#include <network/connection_process.h>
#include <network/server.h>

using namespace connection;
using namespace server;
Process::Process(Server* server,int connection_socket, size_t buffer_sz):
                    buffer_([buffer_sz]()->
                    std::vector<char>
                    {
                        std::vector<char> res;
                        res.resize(buffer_sz);
                        return res;
                    }()),
                    server_(server),
                    connection_socket_(connection_socket){}
Process::Process(Process&& other):server_(other.server_){
    if(this!=&other){
        buffer_.swap(other.buffer_);
        std::swap(instance_,other.instance_);
        thread_.swap(other.thread_);
        std::swap(connection_socket_,other.connection_socket_);
    }
}
Process::~Process(){
    thread_.clear();
    if(connection_socket_!=-1){
        close(connection_socket_);
        connection_socket_=-1;
    }
}
Process& Process::init(Server* server,int connection_socket, size_t buffer_sz){
    ProcessInstance instance =  server->connection_pool_.insert(server->connection_pool_.end(),Process(server,connection_socket,buffer_sz));
    instance->instance_=instance;
    return *instance;
}
void Process::set_buffer_size(size_t sz){
    buffer_.resize(sz);
}
void Process::__send_error_and_close_connection__(ErrorCode err, const char* message){
    network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
    fcntl(connection_socket_,F_SETFL,fcntl(connection_socket_, F_GETFL, 0)|SOCK_NONBLOCK);
    if(msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK)){
        struct pollfd pfd = {.fd=connection_socket_,.events=POLLOUT,.revents=0};
        int poll_return = poll(&pfd,1,1000);
        if(poll_return>0 && (pfd.revents&POLLOUT)){
            msg_err.sendto(connection_socket_,err,ErrorPrint::message(err,message)) && (errno==EAGAIN || errno==EWOULDBLOCK);
        }
    }
    close(connection_socket_);
    connection_socket_=-1;
    return;
}
void Process::__send_error_and_continue__(ErrorCode err,const char* message){
    network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
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
bool Process::__check_and_notify_if_server_inaccessible__(){
    if(server_->status_!=Status::READY){
        __send_error_and_continue__(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"Server is currently inaccessible. Try later");
        return true;
    }
    else return false;
}
void Process::__add_process__(network::TYPE_MESSAGE msg_t){
    thread_.insert(std::thread([this,msg_t](){
        ErrorCode err;
        switch(msg_t){
            case network::TYPE_MESSAGE::SERVER_CHECK:{
                network::Message<network::TYPE_MESSAGE::SERVER_CHECK> msg;
                msg.status = server_->status_;
                break;
            }
            case network::TYPE_MESSAGE::METEO_REQUEST:{
                network::Message<network::TYPE_MESSAGE::METEO_REQUEST> msg;
                Extract hExtract = msg.prepare_and_check_integrity_extractor(err);
                if(err!=ErrorCode::NONE){
                    __send_error_and_continue__(err,"");
                    return err;
                }
                else err = hExtract.execute(); //get output directory
                if(err!=ErrorCode::NONE){
                    __send_error_and_continue__(err,"");
                    return err;
                }
                if(!fs::is_directory(hExtract.out_path()) || !fs::is_regular_file(hExtract.out_path())) //must be directory|regular file (tmp (temporary) in case of internet transaction)
                    __send_error_and_close_connection__(ErrorCode::INTERNAL_ERROR,"Something went wrong (server side)");
                else {
                    network::Message<network::TYPE_MESSAGE::METEO_REPLY> reply_msg;
                    if(fs::is_regular_file(hExtract.out_path()))
                        reply_msg.sendto(connection_socket_,hExtract.out_path());
                    else{
                        for(fs::directory_entry entry:fs::directory_iterator(hExtract.out_path()))
                            reply_msg.sendto(connection_socket_,entry.path());
                    }
                }
                return err;
                break;
            }
            default:{
                __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Unknown request");
                return ErrorCode::INVALID_CLIENT_REQUEST;
            }
        }
    }
    ));
}
ErrorCode Process::launch(){
    size_t to_read = buffer_.size();
    network::TYPE_MESSAGE msg_t;
    while(true){ //вынести switch за пределы цикла
        if(recv(this->connection_socket_,&msg_t,sizeof(msg_t),0)==-1){
            network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
            fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
            msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
            close(connection_socket_);
            return ErrorCode::INTERNAL_ERROR;
        }
        switch(msg_t){
            case network::TYPE_MESSAGE::NONE:
            case network::TYPE_MESSAGE::ERROR:
            case network::TYPE_MESSAGE::METEO_REPLY:
                __send_error_and_continue__(ErrorCode::INVALID_CLIENT_REQUEST,"Invalid request");
                while (to_read<network::sizes_msg_struct[(int)msg_t]-1)
                {
                    if(int readed = recv(this->connection_socket_,&msg_t,sizeof(msg_t),0)==-1){
                        network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
                        fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
                        msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
                        close(connection_socket_);
                        return ErrorCode::INTERNAL_ERROR;
                    }
                    else to_read+=readed;
                }
                continue;
            case network::TYPE_MESSAGE::METEO_REQUEST:{
                if(__check_and_notify_if_server_inaccessible__()){
                    while (to_read<network::sizes_msg_struct[(int)msg_t]-1)
                    {
                        if(int readed = recv(this->connection_socket_,&msg_t,sizeof(msg_t),0)==-1){
                            network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
                            fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
                            msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
                            close(connection_socket_);
                            return ErrorCode::INTERNAL_ERROR;
                        }
                        else to_read+=readed;
                    }
                    continue;
                }
            }
            case network::TYPE_MESSAGE::SERVER_CHECK:{
                break;
            }
            default:{
                __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Unknown request");
                return ErrorCode::INVALID_CLIENT_REQUEST;
            }
        }
        while (to_read<network::sizes_msg_struct[(int)msg_t]-1)
        {
            if(int readed = recv(this->connection_socket_,&msg_t,sizeof(msg_t),0)==-1){
                network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
                fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
                msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
                close(connection_socket_);
                return ErrorCode::INTERNAL_ERROR;
            }
            else to_read+=readed;
        }
        __add_process__(msg_t);
    }
}
ProcessInstance Process::get_instance() const{
    return instance_;
}