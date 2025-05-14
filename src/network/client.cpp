#include <network/client.h>
#include <network/common/def.h>
#include <network/common/message.h>
#include <sys/signalfd.h>

namespace client
{

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Client::Client(const std::string& client_name){
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    if (client_name.empty()) {
        err_=ErrorCode::INVALID_SERVER_NAME_X1;
        return;
    }
    ! memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(""/*client host*/, "" /*port-servive client*/, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }
    for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1){
                perror("client: socket");
                continue;
            }
            if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
                close(sockfd);
                perror("client: connect");
                continue;
            }
            break;
        }
        if (p == NULL){
            fprintf(stderr, "client: failed to connect\n");
            return;
        }
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
        printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo);
        // с этой структурой закончили
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1,0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes]= '\0';
        printf("client: received'%s'\n",buf);
        close(sockfd);
        return ;
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
ErrorCode Client::connect(){
    
}
ErrorCode Client::disconnect(){
    ::close(client_socket_);
    return ErrorCode::NONE;
}
template<>
ErrorCode Client::request<network::TYPE_MESSAGE::METEO_REQUEST>(network::Message<network::TYPE_MESSAGE::METEO_REQUEST> msg){
    ssize_t sent = sizeof(msg);
    err_ = ErrorCode::NONE;
    while(sent!=0){
        ssize_t sent_tmp = send(client_socket_,&msg,sizeof(msg),0);
        if(sent_tmp==-1){
            err_=ErrorPrint::print_error(ErrorCode::SENDING_REQUEST_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            return err_;
        }
    }
    return err_;
}
server::Status Client::server_status() const{
    return server_status_;
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

