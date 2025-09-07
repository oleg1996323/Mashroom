#include <network/client.h>
#include "network/common/message/message_process.h"
#include <sys/signalfd.h>

namespace network
{
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Client::Client(){
}
Client::Client(Client&& other) noexcept{
    if(this!=&other){
        process_ = std::move(other.process_);
        client_thread_.swap(other.client_thread_);
        std::swap(client_,other.client_);
        std::swap(client_socket_,other.client_socket_);
        std::swap(client_interruptor,other.client_interruptor);
        std::swap(err_,other.err_);
        std::swap(temporary_,other.temporary_);
        std::swap(retry_connection_,other.retry_connection_);
    }
}
Client::~Client(){
    if(client_){
        disconnect();            
        std::cout<<"Connection closed: ";
        network::print_ip_port(std::cout,client_.get());
    }
}
void Client::cancel(){
    if(client_thread_.joinable()){
        client_thread_.request_stop();
        if (client_interruptor != -1) {
            uint64_t val = 1;
            write(client_interruptor, &val, sizeof(val));
        }
    }
}
ErrorCode Client::connect(const std::string& host, const std::string& port){
    // if ((rv = getaddrinfo(host_.c_str(), "" /*port-servive client*/, &hints, client_));rv!= 0) {
    //     err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"incorrect service/port number",AT_ERROR_ACTION::CONTINUE);
    //     return err_;
    // }
    uint16_t port_;
    try{
        if(port.empty()){
            port_ = 0;
        }
        else port_ = boost::lexical_cast<uint32_t>(port);
    }
    catch(const boost::bad_lexical_cast& err){
        err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid port number",AT_ERROR_ACTION::CONTINUE);
        return err_;
    }
    if (host.empty()) {
        err_=ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"host name undefined",AT_ERROR_ACTION::CONTINUE);
        return err_;
    }
    
    client_ = std::make_unique<sockaddr_storage>();
    bzero(client_.get(), sizeof(*client_));
    if(auto sock4 = (sockaddr_in*)client_.get();inet_pton(AF_INET,host.c_str(),&sock4->sin_addr)==1){
        sock4->sin_family = AF_INET;
        sock4->sin_port = htons(port_);
    }
    else{
        if(auto* sock6 = (sockaddr_in6*)client_.get();inet_pton(AF_INET6,host.c_str(),&sock6->sin6_addr)==1){
            sock6->sin6_family = AF_INET6;
            sock6->sin6_port = htons(port_);
        }
        else{
            err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid host address",AT_ERROR_ACTION::CONTINUE);
            return err_;
        }
    }
    if ((client_socket_ = socket(AF_UNSPEC, SOCK_STREAM,
        SOL_TCP)) == -1){
        err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client: socket",AT_ERROR_ACTION::CONTINUE);
        return err_;
    }
    if (::connect(client_socket_, (sockaddr*)client_.get(), sizeof(sockaddr)) == -1){
        err_ = ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR_WITH_X1,"client: connect",AT_ERROR_ACTION::CONTINUE,ip_to_text(client_.get())+" "+port_to_text(client_.get()));
        close(client_socket_);
        client_socket_ = -1;
        return err_;
    }
    // for(auto p = client_; p != nullptr; p = p->ai_next) {
    //     if ((sockfd = socket(p->ai_family, p->ai_socktype,
    //         p->ai_protocol)) == -1){
    //         err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"client: socket",AT_ERROR_ACTION::CONTINUE);
    //         continue;
    //     }
    //     if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
    //         err_ = ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR_WITH_X1,"client: connect",AT_ERROR_ACTION::CONTINUE,ip_to_text(p)+" "+port_to_text(p));
    //         close(sockfd);
    //         client_socket_ = -1;
    //         continue;
    //     }
    //     freeaddrinfo(client_);
    //     if(!p){
    //         err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Client initialization\n",AT_ERROR_ACTION::CONTINUE);
    //         return err_;
    //     }
    //     client_ = p;
    //     err_ = ErrorCode::NONE;
    //     break;
    // }
    std::cout<<"Client connected to: ";
        network::print_ip_port(std::cout,client_.get());
    std::cout<<"Ready for request.";
    err_ = ErrorCode::NONE;
    return err_;
}
ErrorCode Client::disconnect(){
    cancel();
    if(server_status_!=server::Status::INACTIVE){
        ::close(client_socket_);
        client_socket_=-1;
    }
    ErrorPrint::print_error(ErrorCode::CONNECTION_CLOSED,"client cannot connect server ",AT_ERROR_ACTION::CONTINUE);
    network::print_ip_port(std::cout,client_.get());
    // if(client_)
    //     freeaddrinfo(client_);
    ::close(client_socket_);

    return ErrorCode::NONE;
}
bool Client::is_connected() const{
    int error = 0;
    socklen_t len = sizeof(error);
    int ret = getsockopt(client_socket_,SOL_SOCKET,SO_ERROR,&error,&len);
    if(ret!=0 || error!=0)
        return false;
    return true;
}

server::Status Client::server_status() const{
    return server_status_;
}
// std::unique_ptr<Client> Client::make_instance(const std::string& client_name,ErrorCode& err){
//     auto result = std::unique_ptr<Client>(new Client(client_name));
//     if(result){
//         ErrorCode err = result->err_;
//         if(err!=ErrorCode::NONE)
//             result.release();
//     }
//     else {
//         ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Client initialization error",AT_ERROR_ACTION::CONTINUE);
//         err = ErrorCode::INTERNAL_ERROR;
//     }
//     return result;
// }
}

