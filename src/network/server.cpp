#include "network/server.h"
#include <network/common/def.h>
#include <program/mashroom.h>
#include <sys/config.h>
#include <sys/log_err.h>
#include <netinet/in.h>



std::unique_ptr<Server> Server::make_instance(ErrorCode& err){
    auto result = std::unique_ptr<Server>(new Server());
    if(result){
        ErrorCode err = result->err_;
        if(err!=ErrorCode::NONE)
            result.release();
    }
    else {
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization error",AT_ERROR_ACTION::CONTINUE);
        err = ErrorCode::INTERNAL_ERROR;
    }
    return result;
}
Server::Server():connection_pool_(*this){
    const server::Settings& settings = Application::config().current_server_setting().settings_;
    Port port = 0;
    try{
        try{
            if(settings.service.empty()){
                if(settings.port.empty()){
                    port = 0;
                }
                else port = boost::lexical_cast<uint32_t>(settings.port);
            }
            else port = boost::lexical_cast<uint32_t>(settings.service);
        }
        catch(const boost::bad_lexical_cast& err){
            err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid port number",AT_ERROR_ACTION::CONTINUE);
            return;
        }

        int last_err = 0;
        server_socket_ = Socket(settings.host,port,Socket::NonBlock|Socket::Stream, Protocol::TCP);
        if(server_socket_.is_valid()){
            err_ = ErrorCode::INTERNAL_ERROR;
            last_err = errno;
            errno=0;
            return;
        }
        server_socket_.set_option(Socket::Options::ReuseAddress,true).
        set_option(Socket::Options::BufferSizeIn,536).
        set_option(Socket::Options::BufferSizeOut,536);
        std::cout<<"Binding to ";
        server_socket_.print_address_info(std::cout);
        server_socket_.bind();
    }
    catch(const std::runtime_error& err){
        err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    catch(const std::invalid_argument& err){
        err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    std::cout<<"Server ready for launching: ";
    server_socket_.print_address_info(std::cout);
}
#include <sys/eventfd.h>
void Server::__launch__(Server* server){
    if(!server)
        return;
    try{
        server->listen(5);
    }
    catch(const std::runtime_error& err){
        server->err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server listening error. "s+err.what(),AT_ERROR_ACTION::CONTINUE);
    }
    
    epoll_event ev;
    ev.events = EPOLLRDNORM | EPOLLRDHUP | EPOLLWRNORM;
    ev.data.fd = server->server_socket_;
    /**
     * @todo set correct size of accepted epoll
     */
    int epollfd = epoll_create(20);
    if(epollfd==-1)
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server epoll init. "s+strerror(errno),AT_ERROR_ACTION::ABORT);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server->server_socket_, &ev) == -1) {
        perror("epoll_ctl: server socket");
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        Mashroom::instance().collapse_server(server);
    }
    server->server_interruptor = eventfd(0,EFD_NONBLOCK);
    if(server->server_interruptor==-1){
        server->err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        return;
    }
    ev.data.fd = server->server_interruptor;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server->server_interruptor, &ev) == -1) {
        perror("epoll_ctl: eventfd");
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR, strerror(errno), AT_ERROR_ACTION::CONTINUE);
        close(epollfd);
        close(server->server_interruptor);
        return;
    }
    server->status_=server::Status::READY;
    printf("server: waiting for connections…\n");
    while(!server->stop_token_.stop_requested()){
        errno = 0;
        int number_epoll_wait;
        if((number_epoll_wait = epoll_wait(epollfd,events.data(),events.size(),-1))==-1 && errno!=EINTR){
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"epoll wait",AT_ERROR_ACTION::CONTINUE);
            close(epollfd);
            return;
        }
        else
        for(int i=0;i<number_epoll_wait;++i){
            if (events.at(i).data.fd == server->server_interruptor) {
                uint64_t val;
                read(server->server_interruptor, &val, sizeof(val));  // Очищаем eventfd
                server->err_=ErrorCode::INTERRUPTED;
            }
            if(events.at(i).data.fd==server->server_socket_){ //if connection requested
                Socket tmp_sock = 0;
                if((tmp_sock = accept(server->server_socket_,(sockaddr*)&another,&sin_size))==-1){
                    server->err_ = ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                    continue;
                }
                else{
                    //add checking allowed client properties
                    //add checking hash key
                    //if not - close connection and continue
                    int res = getpeername(tmp_sock, (struct sockaddr *)&another, &sin_size);
                    char clientip[INET6_ADDRSTRLEN];
                    strcpy(clientip, inet_ntoa(((sockaddr_in*)&another)->sin_addr));
                    std::cout<<std::endl;
                    std::cout<<"Peer IP: ";
                    for(int i=0;i<sin_size;++i)
                        std::cout<<clientip[i];
                    std::cout<<std::endl;
                    if (another.ss_family == AF_INET) {
                        sockaddr_in* addr = (sockaddr_in*)&another;
                        char ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
                        printf("Connecting by IPv4 to: %s:%d\n", ip, ntohs(addr->sin_port));
                    } else if (another.ss_family == AF_INET6) {
                        sockaddr_in6* addr = (sockaddr_in6*)&another;
                        char ip[INET6_ADDRSTRLEN];
                        inet_ntop(AF_INET6, &addr->sin6_addr, ip, INET6_ADDRSTRLEN);
                        printf("Connecting by IPv6 to: [%s]:%d\n", ip, ntohs(addr->sin6_port));
                    }
                    if(__set_no_block__(tmp_sock)!=ErrorCode::NONE)
                        continue;
                }
                server->__new_connection__(events[i].data.fd);
                memset(&another,0,sizeof(sockaddr_storage));
                sin_size = 0;
            }
            else{
                server->connection_pool_.process_connection(events[i].data.fd);
            }
        }
    }
}

void Server::__new_connection__(Socket connected_client){
    if(setsockopt(connected_client,SOL_SOCKET,SO_RCVTIMEO,&Application::config().current_server_setting().settings_.timeout_seconds_,
        sizeof(Application::config().current_server_setting().settings_.timeout_seconds_))==-1){
        Message<network::Server_MsgT::ERROR> msg_err;
        //msg_err.sendto(connected_client,ErrorCode::INTERNAL_ERROR,ErrorPrint::message(ErrorCode::INTERNAL_ERROR,strerror(errno)));
        close(connected_client);
    }
    connection_pool_.add_connection(connected_client);
    std::cout<<"Successfull connection"<<std::endl;
}
network::server::Status Server::get_status() const{
    return status_;
}
void Server::launch(){
    if(server_thread_.joinable())
        server_thread_.join();
    server_thread_ = std::move(std::jthread(__launch__,this));
    stop_token_ = server_thread_.get_stop_token();
    std::cout<<"Server launched: ";
    network::print_ip_port(std::cout,server_.get());
}
void Server::close_connections(bool wait_for_end_connections){
    status_=server::Status::INACTIVE;
    if(wait_for_end_connections){
        stop();
        ::shutdown(server_socket_,SHUT_RDWR);
    }
    else{
        stop();
        ::shutdown(server_socket_,SHUT_RD);
    }
}
void Server::shutdown(bool wait_for_end_connections){
    if(status_!=server::Status::INACTIVE){
        if(!wait_for_end_connections)
            connection_pool_.shutdown_all();
        else connection_pool_.shut_not_processing();
        status_=server::Status::SUSPENDED;
    }
}
Server::~Server(){
    stop();
    if(status_!=server::Status::INACTIVE){
        ::close(server_socket_);
        status_=server::Status::INACTIVE;
        server_socket_=-1;
    }
    char ipstr[INET6_ADDRSTRLEN];
    std::cout<<"Server closed: ";
    network::print_ip_port(std::cout,server_.get());
    // if(server_)
    //     freeaddrinfo(server_);
}
void Server::stop() {
    if(server_thread_.joinable()){
        server_thread_.request_stop();
        if (server_interruptor != -1) {
            uint64_t val = 1;
            write(server_interruptor, &val, sizeof(val));
        }
    }
}