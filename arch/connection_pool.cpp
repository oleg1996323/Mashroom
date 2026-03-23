#include "network/server/connection_pool.h"
#include "server.h"

using namespace network::connection;

network::server::Status ConnectionPool::server_status() const{
    return server_.get_status();
}

network::connection::ConnectionPool::~ConnectionPool(){

}