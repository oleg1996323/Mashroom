#include "network/server/abstractserver.h"

namespace network{
    AbstractServer::AbstractServer(const server::Settings& settings){
        struct sigaction sa;
        socket_in_ = std::make_unique<sockaddr_storage>();
        memset(socket_in_.get(),0,sizeof(sockaddr_storage));
        int yes=1;
        uint16_t port = 0;
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

        if(auto sock4 = (sockaddr_in*)socket_in_.get();inet_pton(AF_INET,settings.host.c_str(),&sock4->sin_addr)==1){
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
        }
        else{
            if(auto* sock6 = (sockaddr_in6*)socket_in_.get();inet_pton(AF_INET6,settings.host.c_str(),&sock6->sin6_addr)==1){
                sock6->sin6_family = AF_INET6;
                sock6->sin6_port = htons(port);
            }
            else{
                err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid host address",AT_ERROR_ACTION::CONTINUE);
                return;
            }
            
        }
        // if(settings.host.size()<=14)
        //     std::strcpy(&ainfo_.ai_addr->sa_data[0],settings.host.c_str());
        // else{
        //     err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"host is longer than 14 characters",AT_ERROR_ACTION::CONTINUE);
        //     return;
        // }
        // if(!settings.service.empty())
        //     service = settings.service.c_str();
        // else if(!settings.port.empty())
        //     service = settings.port.c_str();
        // else{
        //     err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"incorrect service/port number",AT_ERROR_ACTION::CONTINUE);
        //     return;
        // }
        // {
        //     int status=0;
        //     status = getaddrinfo(   settings.host.c_str(),
        //                             service,&ainfo_,&server_);
        //     // status = getaddrinfo(   NULL,
        //     //                         service,&ainfo_,&server_);
        //     if(status!=0){
        //         std::cout<<gai_strerror(status)<<std::endl;
        //         err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
        //         return;
        //     }
        //     //else getnameinfo(ainfo_.ai_addr,ainfo_.ai_addrlen,settings.host.c_str(),settings.host.size(),
        // }
        int last_err = 0;
        socket_ = socket(server_->ss_family,SOCK_STREAM,IPPROTO_TCP);
        if(socket_==-1){
            err_ = ErrorCode::INTERNAL_ERROR;
            last_err = errno;
            errno=0;
            return;
        }
        if (server_->ss_family == AF_INET) {
            sockaddr_in* addr = (sockaddr_in*)socket_in_.get();
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
            printf("Binding to IPv4: %s:%d\n", ip, ntohs(addr->sin_port));
        } else if (server_->ss_family == AF_INET6) {
            sockaddr_in6* addr = (sockaddr_in6*)socket_in_.get();
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, INET6_ADDRSTRLEN);
            printf("Binding to IPv6: [%s]:%d\n", ip, ntohs(addr->sin6_port));
        }
        if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
            err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            socket_=-1;
            last_err = errno;
            errno=0;
            return;
        }
        socklen_t socklen = server_->ss_family==AF_INET?sizeof(sockaddr_in):(server_->ss_family==AF_INET6?sizeof(sockaddr_in6):sizeof(sockaddr_storage));
        if(bind(socket_,(sockaddr*)socket_in_.get(),socklen)==-1){
            ::close(socket_);
            err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server binding "s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
            socket_=-1;
            last_err = errno;
            errno=0;
            return;
        }
        // for(auto ptr_addr = server_;ptr_addr!=nullptr;ptr_addr=ptr_addr->ai_next){
        //     server_socket_ = socket(ptr_addr->ai_family,ptr_addr->ai_socktype,ptr_addr->ai_protocol);
        //     if(server_socket_==-1){
        //         err_ = ErrorCode::INTERNAL_ERROR;
        //         last_err = errno;
        //         errno=0;
        //         continue;
        //     }
        //     if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEPORT, &yes,
        //         sizeof(int)) == -1) {
        //         err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        //         server_socket_=-1;
        //         last_err = errno;
        //         errno=0;
        //         continue;
        //     }
        //     if(bind(server_socket_,ptr_addr->ai_addr,ptr_addr->ai_addrlen)==-1){
        //         close(server_socket_);
        //         err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server binding "s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
        //         server_socket_=-1;
        //         last_err = errno;
        //         errno=0;
        //         continue;
        //     }
        //     err_=ErrorCode::NONE;
        //     server_ = ptr_addr;
        //     break;
        // }
        if(socket_==-1){
            err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization",AT_ERROR_ACTION::CONTINUE);
            return;
        }
        if(__set_no_block__(socket_)!=ErrorCode::NONE){
            err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization - "s+strerror(last_err),AT_ERROR_ACTION::CONTINUE);
            errno = 0;
            return;
        }
        // if(err_!=ErrorCode::NONE){
        //     ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
        //     freeaddrinfo(server_);
        //     return;
        // }
        sa.sa_handler=sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags=SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"sigaction",AT_ERROR_ACTION::ABORT);
        {
            std::cout<<"Server ready for launching: ";
            network::print_ip_port(std::cout,socket_in_.get());
        }
    }
}