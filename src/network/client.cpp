#include "network/client.h"
#include "network/common/message/message_handler.h"
#include <sys/signalfd.h>

namespace network
{
Client::Client(const std::string& host, Port port):
        CommonClient(host,port)
    {}
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

