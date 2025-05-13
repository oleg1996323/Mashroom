// #pragma once
// #include <cstdint>
// #include <string>
// #include <vector>
// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <netdb.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <cstdlib>
// #include <fstream>
// #include "buffer.h"
// #include "client.h"

// namespace server{
//     using Socket = int;
//     using Port = short;
//     using Client_t = struct sockaddr_in;
//     using Server_t = struct sockaddr_in;

//     class Client{
//         void set_address(char* hname, char* sname,struct sockaddr_in* sap,const char* protocol){
//             struct servent* sp;
//             struct hostent* hp;
//             Port c_port_;
//             char* endptr;
//             bzero(sap,sizeof(*sap));
//             sap->sin_family = AF_INET;
//             if(hname!=NULL){
//                 if(!inet_aton(hname,&sap->sin_addr)){
//                     hp = gethostbyname(hname);
//                     if(hp==NULL){
//                         exit(1);
//                     }
//                     sap->sin_addr = *(struct in_addr*)hp->h_addr;
//                 }
//             }
//             else
//                 sap->sin_addr.s_addr = htonl(INADDR_ANY);

//             c_port_ = strtol(sname,&endptr,0);
//             if(*endptr=='\0')
//                 sap->sin_port = htons(c_port_);
//             else{
//                 sp = getservbyname(sname,protocol);
//                 if(sp==NULL)
//                     exit(1);
//                 sap->sin_port = sp->s_port;
//             }
//         }
//         public:
//         Client(char* hname,char* sname){
//             set_address(hname,sname,&client_,"tcp");
//         }

//         void launch(){
//             s_sock_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//             if(s_sock_<0)
//                 exit(1); //invalid socket
//             else{
//                 int val = 120;
//                 if(setsockopt(s_sock_,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)))
//                     exit(1); //error of setsockopt
//                 val = 1;
//                 if(setsockopt(s_sock_,SOL_SOCKET,SO_KEEPALIVE,&val,sizeof(val)))
//                     exit(1); //error of setsockopt
//                 val = 8096;
//                 if(setsockopt(s_sock_,SOL_SOCKET,SO_RCVBUF,&val,sizeof(val)))
//                     exit(1); //error of setting receiving buffer size
//                 val = 1024;
//                 if(setsockopt(s_sock_,SOL_SOCKET,SO_SNDBUF,&val,sizeof(val)))
//                     exit(1); //error of setting sending buffer size
//                 val = 120;
//                 if(setsockopt(s_sock_,SOL_TCP,TCP_USER_TIMEOUT,&val,sizeof(val))) //to client socket
//                     exit(1); //error of setting user timeout
//                 // val = 5;
//                 // if(setsockopt(s_sock_,SOL_TCP,TCP_SYNCNT,&val,sizeof(val)))
//                 //     exit(1);
//                  val = 5000;
//                 if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPIDLE,&val,sizeof(val)))
//                     exit(1); //first check signal after non-action
//                 val = 5000;
//                 if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPINTVL,&val,sizeof(val)))
//                     exit(1);
//                 val = 5000;
//                 if(setsockopt(s_sock_,SOL_TCP,TCP_KEEPCNT,&val,sizeof(val)))
//                     exit(1);
//                 do{
//                     if(connect(c_sock_,(struct sockaddr*)&client_,sizeof(client_))){
//                         close(c_sock_);
//                         exit(1); //error at accept
//                     }
//                     execute();
//                     close(c_sock_);
//                 }while(1);
//             }
//         }

//         void new_connection(mashroom::Date from,mashroom::Date to,mashroom::Coord pos, mashroom::DATA_OUT fmt){
//             std::error_code err_code;
//             fs::path tmp_path = fs::temp_directory_path(err_code);
//             mashroom::MashroomLink::extract_by_pos_func()("/home/oster/out_test",tmp_path,from,to,pos,mashroom::GRIB, fmt);
//             execute();
//         }

//         void execute(){
//             ssize_t rc;
//             rc=recv(c_sock_,buf,sizeof(buf),0);
//             if(rc<0){
//                 buf[0]='0';
//                 ssize_t sent = send(c_sock_,buf,1,0);
//                 if(sent<0){
//                     std::cerr<<"Fatal error. Abort."<<std::endl;
//                     exit(1);
//                 }
//                 return;
//             }
//             else{
//                 fs::path nf_path(buf);
//                 std::ofstream new_file(nf_path);
//                 if(!new_file.is_open()){
//                     std::cout<<std::string("Error at openning file ")+buf<<std::endl;
//                     exit(1);
//                 }
//                 while(1){
//                     rc=recv(c_sock_,buf,sizeof(buf),0);
//                     if(rc<0){
//                         std::cerr<<"Fatal error. Abort."<<std::endl;
//                         new_file.close();
//                         fs::remove(nf_path);
//                         exit(1);
//                         return;
//                     }
//                     else if(rc==0){
//                         continue;
//                     }
//                     else{
//                         new_file.write(buf,rc);
//                     }
//                 }
//             }
//         }

//         private:
//         Client_t client_;
//         socklen_t peerlen_;
//         Server_t server_;
//         Socket s_sock_;
//         Socket c_sock_;
//         char buf[8096];
//     };
// }