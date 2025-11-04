#include "network/client/connection_process.h"
#include "network/client.h"

using namespace network::connection;

Process<network::Client>::Process(Process&& other) noexcept:cv_(other.cv_){
    if(this!=&other)
        *this = std::move(other);
}
Process<Client>& Process<Client>::operator=(Process&& other) noexcept{
    if(this!=&other){
        cv_=other.cv_;
        AbstractProcess::operator=(std::move(other));
    }
    return *this;
}