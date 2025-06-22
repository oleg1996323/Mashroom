#include <network/client/connection_process.h>

using namespace network::connection;

Process<network::Client>::Process(Process&& other) noexcept{
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