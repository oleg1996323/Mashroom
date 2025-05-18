#include "network/server.h"
#include <network/common/def.h>
#include <program/mashroom.h>
#include <sys/config.h>
#include <sys/log_err.h>

using namespace server;

void Server::sigchld_handler(int s){
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

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
ErrorCode Server::__set_no_block__(int socket){
    if(int flags = fcntl(socket,F_GETFL,0)==-1){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"getting socket flags",AT_ERROR_ACTION::CONTINUE);
        close(socket);
        return ErrorCode::INTERNAL_ERROR;
    }
    else{
        if(fcntl(socket,F_SETFL,flags|O_NONBLOCK)==-1){
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"setting socket flags",AT_ERROR_ACTION::CONTINUE);
            close(socket);
            return ErrorCode::INTERNAL_ERROR;
        }
    }
    return ErrorCode::NONE;
}
Server::Server():connection_pool_(*this){
    struct addrinfo ainfo_;
    struct sigaction sa;
    memset(&ainfo_,0,sizeof(ainfo_));
    ainfo_.ai_family = AF_INET;
    ainfo_.ai_flags = AI_PASSIVE;
    ainfo_.ai_socktype = SOCK_STREAM;
    const char* service;
    int yes=1;
    if(!Application::config().current_server_setting().settings_.service.empty())
        service = Application::config().current_server_setting().settings_.service.c_str();
    else if(!Application::config().current_server_setting().settings_.port.empty())
        service = Application::config().current_server_setting().settings_.port.c_str();
    else{
        err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"incorrect service/port number",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    {
        int status=0;
        status = getaddrinfo(   Application::config().current_server_setting().settings_.host.c_str(),
                                service,&ainfo_,&server_);
        // status = getaddrinfo(   NULL,
        //                         service,&ainfo_,&server_);
        if(status!=0){
            std::cout<<gai_strerror(status)<<std::endl;
            err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
            return;
        }
    }
    for(auto ptr_addr = server_;ptr_addr!=nullptr;ptr_addr=ptr_addr->ai_next){
        server_socket_ = socket(ptr_addr->ai_family,ptr_addr->ai_socktype,ptr_addr->ai_protocol);
        if(server_socket_==-1){
            err_ = ErrorCode::INTERNAL_ERROR;
            errno=0;
            continue;
        }
        if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
            err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            server_socket_=-1;
            errno=0;
            continue;
        }
        if(bind(server_socket_,ptr_addr->ai_addr,ptr_addr->ai_addrlen)==-1){
            close(server_socket_);
            err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server binding"s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
            server_socket_=-1;
            errno=0;
            continue;
        }
        err_=ErrorCode::NONE;
        server_ = ptr_addr;
        break;
    }
    if(server_socket_==-1){
        err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    if(__set_no_block__(server_socket_)!=ErrorCode::NONE){
        err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization - "s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
        errno = 0;
        return;
    }
    if(err_!=ErrorCode::NONE){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        freeaddrinfo(server_);
        return;
    }
    sa.sa_handler=sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"sigaction",AT_ERROR_ACTION::ABORT);
    {
        std::cout<<"Server ready for launching: ";
        network::print_ip_port(std::cout,server_);
    }
}
#include <sys/eventfd.h>
void Server::__launch__(Server* server){
    if(!server)
        return;
    if(listen(server->server_socket_,5)==-1){
        server->err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server listening error. "s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
        return;
    }
    socklen_t sin_size;
    sockaddr_storage another;
    memset(&another,0,sizeof(sockaddr_storage));
    
    std::vector<epoll_event> events = define_epoll_event();
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server->server_socket_;
    int epollfd = epoll_create1(0);
    if(epollfd==-1)
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server epoll init. "s+strerror(errno),AT_ERROR_ACTION::ABORT);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server->server_socket_, &ev) == -1) {
        perror("epoll_ctl: server socket");
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        hProgram->collapse_server(server);
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
    server->status_=Status::READY;
    printf("server: waiting for connections…\n");
    while(!server->stop_token_.stop_requested()){
        errno = 0;
        int number_epoll_wait;
        if((number_epoll_wait = epoll_wait(epollfd,events.data(),events.size(),-1))==-1 && errno!=EINTR){
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"epoll wait",AT_ERROR_ACTION::CONTINUE);
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
                if((tmp_sock = accept(server->server_socket_,(struct sockaddr*)&another,&sin_size))==-1){
                    server->err_ = ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                    continue;
                }
                else{
                    //add checking allowed client properties
                    //add checking hash key
                    //if not - close connection and continue
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
        server::Message<server::TYPE_MESSAGE::ERROR> msg_err;
        msg_err.sendto(connected_client,ErrorCode::INTERNAL_ERROR,ErrorPrint::message(ErrorCode::INTERNAL_ERROR,strerror(errno)));
        close(connected_client);
    }
    connection_pool_.add_connection(connected_client);
}
server::Status Server::get_status() const{
    return status_;
}
void Server::launch(){
    if(server_thread_.joinable())
        server_thread_.join();
    server_thread_ = std::move(std::jthread(__launch__,this));
    stop_token_ = server_thread_.get_stop_token();
    std::cout<<"Server launched: ";
    network::print_ip_port(std::cout,server_);
}
void Server::close_connections(bool wait_for_end_connections){
    ::shutdown(server_socket_,SHUT_RDWR);
}
void Server::shutdown(bool wait_for_end_connections){
    if(status_!=Status::INACTIVE){
        if(!wait_for_end_connections)
            connection_pool_.shutdown_all();
        else connection_pool_.shut_not_processing();
        status_=Status::SUSPENDED;
    }
}
Server::~Server(){
    stop();
    if(status_!=Status::INACTIVE){
        ::close(server_socket_);
        status_=Status::INACTIVE;
        server_socket_=-1;
    }
    char ipstr[INET6_ADDRSTRLEN];
    std::cout<<"Server closed: ";
    network::print_ip_port(std::cout,server_);
    if(server_)
        freeaddrinfo(server_);
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