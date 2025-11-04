#include "network/client.h"
#include "network/common/message/message_process.h"
#include <sys/signalfd.h>

namespace network
{
Client::Client(const std::string& host, Port port): CommonClient(host,port),
                                                    cv_(std::make_shared<std::condition_variable_any>())
                                                    {}
Client::Client(Client&& other) noexcept:CommonClient(std::move(other)){
    if(this!=&other){
        process = std::move(other.process);
        socket_.swap(other.socket_);
        mprocess_ = std::move(other.mprocess_);
        server_status_ = other.server_status_;
        cv_.swap(other.cv_);
    }
}
Client& Client::operator=(Client&& other) noexcept{
    if(this!=&other){
        process = std::move(other.process);
        socket_.swap(other.socket_);
        mprocess_ = std::move(other.mprocess_);
        server_status_ = other.server_status_;
        cv_.swap(other.cv_);
    }
    return *this;
}
bool Client::operator==(const Client& other) const noexcept{
    return socket_ && other.socket_ && (*other.socket_.get())==(*socket_.get());
}

Client::~Client(){
    if(socket_){
        std::cout<<"Closing connection: ";
        socket_->print_address_info(std::cout);
        std::cout<<"Connection closed"<<std::endl;
    }
}
server::Status Client::server_status() const{
    return server_status_;
}
}

