#include "network/server.h"
#include <network/common/def.h>
#include <program/mashroom.h>
#include <sys/config.h>
#include <sys/log_err.h>
#include <netinet/in.h>



std::unique_ptr<Server> Server::make_instance(const server::Settings& settings){
    auto result = std::unique_ptr<Server>(
        new Server(settings));
    if(!result)
        throw std::runtime_error("Server initialization error");
    return result;
}
Server::Server(const server::Settings& settings):CommonServer(settings,*this){
    std::cout<<"Server ready for launching: ";
    this->socket().print_address_info(std::cout);
}

void Server::after_accept(Socket& socket){
    try{
        socket.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=5,.tv_usec = 0},Socket::Options::TimeOutIn)));
        socket.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=5,.tv_usec = 0},Socket::Options::TimeOutOut)));
        using Event_t = Multiplexor::Event;
        modify_connection(socket,Event_t::EdgeTrigger|Event_t::In|Event_t::HangUp);
        std::cout<<"Connecting ";
        socket.print_address_info(std::cout);
    }
    catch(const std::runtime_error& err){
        std::cout<<"Error after accepting connection"<<std::endl;
        return;
    }
    return;
}
void Server::after_listen(){
    status_ = server::Status::READY;
}
network::server::Status Server::get_status() const{
    return status_;
}
Server::~Server(){
    close(false,0);
}