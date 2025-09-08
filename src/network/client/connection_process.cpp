#include <network/client/connection_process.h>
#include "network/client.h"

using namespace network::connection;

Process<network::Client>::Process(Process&& other) noexcept:Process(other.client_){
    this->swap(other);
}
Process<Client>& Process<Client>::operator=(Process&& other) noexcept{
    this->swap(other);
    return *this;
}
void Process<Client>::swap(Process<Client>& other) noexcept{
    if(this!=&other){
        std::swap(client_,other.client_);
        std::swap(mprocess_,other.mprocess_);
    }
}

Socket Process<Client>::__socket__() const noexcept{
    if(client_)
        return client_->client_socket_;
    else return -1;
}