#include <network/client/connection_process.h>

using namespace network::connection;
using namespace network::client;
Process<Client>::Process(Process&& other) noexcept{
    this->swap(other);
}
Process<Client>& Process<Client>::operator=(Process&& other) noexcept{
    this->swap(other);
    return *this;
}
void Process<Client>::swap(Process<Client>& other) noexcept{
    if(this!=&other){
        //TODO
    }
}