#include "network/server.h"
#include <network/def.h>
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
std::ostream& Server::print_ip_port(std::ostream& stream,addrinfo* addr){
    char ipstr[INET6_ADDRSTRLEN];
    if(addr->ai_family==AF_INET){
        sockaddr_in* a = (sockaddr_in *)addr->ai_addr;
        stream<<"IPv4="<<inet_ntop(addr->ai_family, &a->sin_addr, ipstr, sizeof(ipstr))<<
        " port="<<ntohs(a->sin_port)<<std::endl;
        return stream;
    }
    else {
        sockaddr_in6* a = (sockaddr_in6 *)addr->ai_addr;
        stream<<"IPv6="<<inet_ntop(addr->ai_family, &a->sin6_addr, ipstr, sizeof(ipstr))<<
        " port="<<ntohs(a->sin6_port)<<std::endl;
        return stream;
    }
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
            err_ = err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server initialization: "s+strerror(errno),AT_ERROR_ACTION::CONTINUE);
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
        print_ip_port(std::cout,server_);
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
        network::Message<network::TYPE_MESSAGE::ERROR> msg_err;
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
    print_ip_port(std::cout,server_);
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
    print_ip_port(std::cout,server_);
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

// void server::Server::set_socket(){
//     s_sock_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//     if(s_sock_<0)
//         exit(1); //invalid socket
//     else{
//         int val = 1;
//         if(setsockopt(s_sock_,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)))
//             exit(1); //error of setsockopt
//         val = 1;
//         if(setsockopt(s_sock_,SOL_SOCKET,SO_KEEPALIVE,&val,sizeof(val)))
//             exit(1); //error of setsockopt
//         val = 1024;
//         if(setsockopt(s_sock_,SOL_SOCKET,SO_RCVBUF,&val,sizeof(val)))
//             exit(1); //error of setting receiving buffer size
//         val = 8096;
//         if(setsockopt(s_sock_,SOL_SOCKET,SO_SNDBUF,&val,sizeof(val)))
//             exit(1); //error of setting sending buffer size
//         //val = 120;
//         //if(setsockopt(s_sock_,SOL_TCP,TCP_USER_TIMEOUT,&val,sizeof(val))) //to client socket
//             //exit(1); //error of setting user timeout
//         // val = 5;
//         // if(setsockopt(s_sock_,SOL_TCP,TCP_SYNCNT,&val,sizeof(val)))
//         //     exit(1);
//         // val = 5;
//         // if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPIDLE,&val,sizeof(val)))
//         //     exit(1); //first check signal after non-action
//         // val = 5;
//         // if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPINTVL,&val,sizeof(val)))
//         //     exit(1);
//         // val = 5;
//         // if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPCNT,&val,sizeof(val)))
//         //     exit(1);

//         if(bind(s_sock_,(struct sockaddr*)&server_,sizeof(server_)))
//             exit(1); //error at bind
        
//         if(listen(s_sock_,20))
//             exit(1); //error at listen
//     }
// }

// void server::Server::launch_struct(){
//     set_socket();
//     Header data;
//     do{
//         c_sock_ = accept(s_sock_,(struct sockaddr*)&client_,&peerlen_); //peerlen add to client socket structure (make clients properties)
//         if(c_sock_<0){
//             std::cout<<"Error connection with client: IP = "<<get_ip4_str(client_.sin_addr.s_addr)<<"\nAbort connection."<<std::endl;
//             return;
//         }
//         else{
//             bzero(buf,1024);
//             int receiver_retry_count = 5;
//             size_t data_requested = 0;
//             size_t read_data = 0;
//             ssize_t buf_offset = 0;
//             while(1){
//                 ssize_t received= recv(c_sock_,&buf+buf_offset,1024-buf_offset,0);
//                 if(received==-1){
//                     std::cout<<"Error at data receiving from "<<get_ip4_str(client_.sin_addr.s_addr)<<"\nAbort connection."<<std::endl;
//                     close(c_sock_);
//                     return;
//                 }
//                 else if(received==0){
//                     if(receiver_retry_count==0){
//                         std::cout<<"Connection closed with client: IP = "<<get_ip4_str(client_.sin_addr.s_addr)<<std::endl;
//                         close(c_sock_);
//                         return;
//                     }
//                     else{
//                         --receiver_retry_count;
//                         sleep(1);
//                         continue;
//                     }
//                 }
//                 else{
//                     if(read_data==0 || std::holds_alternative<std::monostate>(data) || buf_offset>0){
//                         if(read_data>max_hdr_size){
//                             std::cout<<"Error at reading data header from "<<get_ip4_str(client_.sin_addr.s_addr)<<"\nConnection closed."<<std::endl;
//                             close(c_sock_);
//                             return;
//                         }
//                         switch(buf[0]){
//                         case 1:
//                             data = hdr<TYPE_MESSAGE::SIZE_DATA>();
//                             break;
//                         case 2:
//                             data = hdr<TYPE_MESSAGE::DATE_POS_REQ>();
//                             break;
//                         case 3:
//                             data = hdr<TYPE_MESSAGE::DATE_POS_REQ>();
//                             break;
//                         };
//                         read_data+=received;
//                         if(buf_offset+received<data.get_size()){
//                             buf_offset+=data.get_size()-received;
//                             continue;
//                         }
//                         else{
//                             std::memcpy(&data,buf,data.get_size());
//                             buf_offset = data.get_size();
//                         }
//                     }
//                     if(read_data>buf_offset){
//                         switch(data.type()){
//                             case TYPE_MESSAGE::SIZE_DATA:
//                                 break;
//                             case TYPE_MESSAGE::DATE_POS_REQ:
//                                 break;
//                             case TYPE_MESSAGE::CONFIRMATION:
//                                 break;
//                             case TYPE_MESSAGE::CHECK_DATA:
//                                 break;
//                         }
//                     }
//                     buf_offset=0;
//                     continue;
//                 }
//             }
//         }
//     }while(1);
// }

// void server::Server::new_connection(const hdr<TYPE_MESSAGE::DATE_POS_REQ>& data){
//     std::error_code err_code;
//     fs::path tmp_path = fs::temp_directory_path(err_code);
//     mashroom::MashroomLink::extract_by_pos_func()("/home/oster/out_test",tmp_path,data.from,data.to,data.pos,mashroom::GRIB, (mashroom::DATA_OUT)(mashroom::DATA_OUT::TXT_F&mashroom::DATA_OUT::ARCHIVED));
//     send_date_pos_data(data);
//     close(c_sock_);
// }

// void server::Server::send_date_pos_data(const hdr<TYPE_MESSAGE::DATE_POS_REQ>& data, const fs::path& filename){
//     std::streamsize read_count = 0;
//     std::ifstream file(file_to_send,std::ios::binary);
//     if(!file.is_open()){
//         std::cout<<"Cannot open file "<<file_to_send<<std::endl;
//         return;
//     }
//     {
//         size_t fn_sz = file_to_send.filename().string().size();
//         memcpy(buf,file_to_send.filename().c_str(),file_to_send.filename().string().size());
//         void* p = buf;
//         while(fn_sz>0){
//             ssize_t sent = send(c_sock_,p,read_count,0);
//             read_count-=sent;
//             p+=sent;
//         }
//         while(1){
//             ssize_t ctnl_done = recv(s_sock_,buf,read_count,0);
//             if(ctnl_done<0) //if socket closed, then -1
//                 exit(1);
//             else if(ctnl_done==1){
//                 if(buf[0]=='1')
//                     break;
//                 else if(buf[0]=='0'){
//                     std::cout<<"interrupted by client"<<std::endl;
//                     exit(1);
//                 }
//                 else{
//                     std::cout<<"Fatal error. Unknown return value from client"<<std::endl;
//                     exit(1);
//                 }
//             }
//         }
//     }
//     while(1){
//         file.read(buf,sizeof(buf));
//         read_count = file.gcount();
//         if(!file.good() || !file.eof()){
//             std::cerr<<"Error at reading file "<<file_to_send<<std::endl;
//             exit(1);
//         }
//         else if(file.eof())
//             break;
//         void* p = buf;
//         while(read_count>0){
//             ssize_t sent = send(c_sock_,p,read_count,0);
//             read_count-=sent;
//             p+=sent;
//         }
//     }
// }

// void server::Server::launch_args(){
//     set_socket();
//     do{
//         c_sock_ = accept(s_sock_,(struct sockaddr*)&client_,&peerlen_);
//         if(c_sock_<0){
//             close(s_sock_);
//             exit(1); //error at accept
//         }
//         else{
//             //-from 1991/01/01/00 -to 1992/01/01/01 -pos 45.0:45.0
//             mashroom::Date from;
//             mashroom::Date to;
//             mashroom::Coord pos;
//             mashroom::DATA_OUT format=mashroom::ARCHIVED;
//             bzero(buf,1024);
//             ssize_t r = recv(c_sock_,&buf,1024,0);
//             if(r>0){
//                 char* str_1 = buf;
//                 char* str_1_save = nullptr;
//                 char* token_1 = nullptr;
//                 token_1 = strtok_r(str_1," \r\n",&str_1_save);
//                 while(token_1){
//                     if(strlen(token_1)!=0){
//                         bool is_from = false;
//                         bool is_to = false;
//                         bool is_pos = false;
//                         bool is_fmt = false;
//                         bool from_def = false;
//                         bool to_def = false;
//                         bool pos_def = false;
//                         bool fmt_def = false;
//                         if(strcmp(token_1,"-from") == 0)
//                             is_from = true;
//                         else if(strcmp(token_1,"-to") == 0)
//                             is_to = true;
//                         else if(strcmp(token_1,"-pos") == 0)
//                             is_pos = true;
//                         else if(strcmp(token_1,"-fmt") == 0)
//                             is_fmt = true;
//                         else{
//                             std::cout<<"Unknown operator"<<std::endl;
//                             exit(1);
//                         }

//                         if(is_from || is_to){
//                             token_1 = strtok_r(nullptr," \r\n",&str_1_save);
//                             char* str_2 = token_1;
//                             char* str_2_save = nullptr;
//                             char* token_2 = strtok_r(str_2,"/\r\n",&str_2_save);
//                             if(strlen(token_1)!=0){
//                                 for(int i=0;i<4;++i,token_2=strtok_r(nullptr,"/\r\n",&str_2_save)){
//                                     try{
//                                         switch(i){
//                                             case 0:
//                                                 if(is_from && !from_def)
//                                                     from.year = std::stoi(token_2);
//                                                 else if(is_to && !to_def)
//                                                     to.year = std::stoi(token_2);
//                                                 break;
//                                             case 1:
//                                                 if(is_from && !from_def)
//                                                     from.month = std::stoi(token_2);
//                                                 else if(is_to && !to_def)
//                                                     to.month = std::stoi(token_2);
//                                                 break;
//                                             case 2:
//                                                 if(is_from && !from_def)
//                                                     from.day = std::stoi(token_2);
//                                                 else if(is_to && !to_def)
//                                                     to.day = std::stoi(token_2);
//                                                 break;
//                                             case 3:
//                                                 if(is_from && !from_def)
//                                                     from.hour = std::stoi(token_2);
//                                                 else if(is_to && !to_def)
//                                                     to.hour = std::stoi(token_2);
//                                                 break;
//                                             default:
//                                                 exit(1);
//                                         }
//                                     }
//                                     catch(const std::invalid_argument& err){
//                                         std::cout<<"Error conversion"<<std::endl;
//                                     }
//                                 }
//                                 if(is_from){
//                                     from_def = true;
//                                     is_from = false;
//                                 }
//                                 else if(is_to){
//                                     to_def = true;
//                                     is_to = false;
//                                 }
//                                 else if(is_pos){
//                                     pos_def = true;
//                                     is_pos = false;
//                                 }
//                             }
//                         }
//                         else if(is_pos){
//                             token_1 = strtok_r(nullptr," \r\n",&str_1_save);
//                             char* str_2 = token_1;
//                             char* str_2_save = nullptr;
//                             char* token_2 = strtok_r(str_2,":\r\n",&str_2_save);
//                             if(strlen(token_1)!=0){
//                                 for(int i=0;i<2;++i,token_2=strtok_r(nullptr,":\r\n",&str_2_save)){
//                                     try{
//                                         switch(i){
//                                             case 0:
//                                                 pos.lat_ = std::stof(token_2);
//                                                 break;
//                                             case 1:
//                                                 pos.lon_ = std::stof(token_2);
//                                                 break;
//                                             default:
//                                                 exit(1);
//                                         }
//                                     }
//                                     catch(const std::invalid_argument& err){
//                                         std::cout<<"Error conversion"<<std::endl;
//                                         exit(1);
//                                     }
//                                 }
//                                 pos_def = true;
//                                 is_pos = false;
//                             }
//                         }
//                         else if(is_fmt){
//                             token_1 = strtok_r(nullptr," \r\n",&str_1_save);
//                             if(strlen(token_1)!=0){
//                                 if(strcmp(token_1,"txt"))
//                                     format=(mashroom::DATA_OUT)(format&mashroom::DATA_OUT::TXT_F);
//                                 else if(strcmp(token_1,"bin"))
//                                     format=(mashroom::DATA_OUT)(format&mashroom::DATA_OUT::BIN_F);
//                                 else if(strcmp(token_1,"grib"))
//                                     format=(mashroom::DATA_OUT)mashroom::DATA_OUT::GRIB_F;
//                                 fmt_def = true;
//                                 is_fmt = false;
//                             }
//                         }
//                         else{
//                             std::cout<<"Error at parsing. Abort connection."<<std::endl;
//                             exit(1);
//                         }
//                     }
//                     token_1 = strtok_r(nullptr," \r\n",&str_1_save);
//                 }
//             }   
//             new_connection(from,to,pos,format);
//         }
//     }while(1);
// }