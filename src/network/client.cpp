#include <network/client.h>

namespace client
{
    Client::Client(){
        //think about initialization
    }
    Client::~Client(){
        disconnect();            
        client_socket_=-1;
        char ipstr[INET6_ADDRSTRLEN];
        std::cout<<"Connection closed: ";
        network::print_ip_port(std::cout,client_);
        if(client_)
            freeaddrinfo(client_);
    }
    void Client::cancel(){

    }
    void Client::disconnect(){
        ::close(client_socket_);
    }
    void Client::request(){

    }
    void Client::server_status(){

    }
    std::unique_ptr<Client> Client::make_instance(ErrorCode& err){
        auto result = std::unique_ptr<Client>(new Client());
        if(result){
            ErrorCode err = result->err_;
            if(err!=ErrorCode::NONE)
                result.release();
        }
        else {
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Client initialization error",AT_ERROR_ACTION::CONTINUE);
            err = ErrorCode::INTERNAL_ERROR;
        }
        return result;
    }
}

