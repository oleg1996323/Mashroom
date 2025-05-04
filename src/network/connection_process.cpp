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
        std::swap(msg_t_,other.msg_t_);
    }
}
Process::~Process(){
    if(thread_)
        thread_->join();
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
    if(server_->status_==Status::INACTIVE){
        __send_error_and_continue__(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"Server is currently inaccessible. Try later");
        return true;
    }
    else return false;
}
ErrorCode Process::launch(){
    thread_ = std::make_unique<std::thread>([this](){
        ErrorCode err;
        size_t to_read = buffer_.size();
        while(to_read>0) //вынести switch за пределы цикла
            if(recv(this->connection_socket_,&msg_t_,1,0)==-1){
                network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
                fcntl(connection_socket_,F_SETFL,SOCK_NONBLOCK);
                msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
                close(connection_socket_);
            }
            else{
                if(msg_t_==network::TYPE_MESSAGE::SERVER_CHECK){
                    network::Message<network::TYPE_MESSAGE::SERVER_CHECK> msg;
                    msg.status = server_->status_;
                    return ErrorCode::NONE;
                }
                else{
                    if(__check_and_notify_if_server_inaccessible__())
                        return ErrorCode::SERVER_RECEIVING_MSG_ERROR;
                    switch(msg_t_){
                        case network::TYPE_MESSAGE::NONE:
                            __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Request not sent");
                            return ErrorCode::INVALID_CLIENT_REQUEST;
                            break;
                        case network::TYPE_MESSAGE::ERROR:
                            __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Error message can by only replied");
                            return ErrorCode::INVALID_CLIENT_REQUEST;
                            break;
                        case network::TYPE_MESSAGE::METEO_REPLY:
                            __send_error_and_close_connection__(ErrorCode::INVALID_CLIENT_REQUEST,"Data reply message can't be sent as request");
                            return ErrorCode::INVALID_CLIENT_REQUEST;
                            break;
                        case network::TYPE_MESSAGE::METEO_REQUEST:{
                            network::Message<network::TYPE_MESSAGE::METEO_REQUEST> msg;
                            Extract hExtract = msg.prepare_and_check_integrity_extractor(err);
                            if(err!=ErrorCode::NONE){
                                __send_error_and_close_connection__(err,"");
                                return err;
                            }
                            else err = hExtract.execute(); //get output directory
                            if(err!=ErrorCode::NONE){
                                __send_error_and_close_connection__(err,"");
                                return err;
                            }
                            if(!fs::is_directory(hExtract.out_path()) || !fs::is_regular_file(hExtract.out_path())) //must be directory|regular file (tmp (temporary) in case of internet transaction)
                                __send_error_and_close_connection__(ErrorCode::INTERNAL_ERROR,"Something went wrong (server side)");
                            else {
                                network::Message<network::TYPE_MESSAGE::METEO_REPLY> reply_msg;
                                if(server_->interput_transactions_)
                                    shutdown(connection_socket_,SHUT_RDWR);
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
                            //Process::init(connection_pool_,connected_client,def_type,network::sizes_msg_struct[(int)def_type]-sizeof(def_type));
                    }
                }
            }
            if(recv(connection_socket_,buffer_.data(),buffer_.size(),0)==-1){
                network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
                msg_err.sendto(connection_socket_,ErrorCode::SERVER_RECEIVING_MSG_ERROR,ErrorPrint::message(ErrorCode::SERVER_RECEIVING_MSG_ERROR,strerror(errno)));
                close(connection_socket_);
                return ErrorCode::INTERNAL_ERROR;
            }
        switch (msg_t_)
        {
        case network::TYPE_MESSAGE::METEO_REQUEST:{
            network::Message<network::TYPE_MESSAGE::METEO_REQUEST> msg;
            break;
        }
        default:
            break;
        }
    });
}
ProcessInstance Process::get_instance() const{
    return instance_;
}