#include "commonclient.h"
#include "commonserver.h"
#include <gtest/gtest.h>
#include "send.h"
#include "receive.h"

using namespace network;

void send_byte(std::stop_token stop,const Socket& socket){
    int tryings = 100;
    for(int count=0;count<5 && tryings>=0;--tryings){
        std::array<char,1000> arr;
        std::ranges::fill_n(arr.begin(),1000,'\1');
        if(send(socket,arr)!=-1){
            std::cout<<"Sending successed"<<std::endl;   
            ++count;
        }
        else{
            std::cout<<"Sending failed"<<std::endl;
            continue;
        }
    }
}

class ProcessPing:public AbstractProcess<ProcessPing>{
};

class ConnectionPool:public AbstractConnectionPool<ProcessPing>{
    FRIEND_TEST(Client_server,ping);
    int count_recv = 0;
    virtual void execute(std::stop_token,const Socket& socket) override;
};

void ConnectionPool::execute(std::stop_token,const Socket& socket){
    std::vector<char> buffer;
    int tryings = 100;
    while(tryings-->=0 && count_recv<5){
        buffer.clear();
        buffer.resize(1000);
        if(receive(socket,buffer,1000)!=-1 && std::all_of(buffer.begin(),buffer.end(),[](char ch){return ch == '\1';})){
            std::cout<<"Receiving successed"<<std::endl;
            ++count_recv;
        }
        else std::cout<<"Receiving failed"<<std::endl;
    }
    if(tryings==0 && count_recv<5)
        throw std::runtime_error("Test failed");
}

class Server:public CommonServer<ConnectionPool>{
    FRIEND_TEST(Client_server,ping);
    void after_accept(network::Socket& socket) override{
        try{
            socket.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=3,.tv_usec = 0},Socket::Options::TimeOutIn)));
            socket.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=3,.tv_usec = 0},Socket::Options::TimeOutOut)));
            socket.set_no_block(false);
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
};

class Client:public CommonClient<ProcessPing>{
    virtual void after_connect(Socket& sock) override{
        sock.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=3,.tv_usec = 0},Socket::Options::TimeOutIn)));
        sock.set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=3,.tv_usec = 0},Socket::Options::TimeOutOut)));
        sock.set_no_block(false);
        std::cout<<"Connected to: ";
        sock.print_address_info(std::cout);
    }
};

TEST(Client_server,ping){
    server::Settings settings;
    settings.host = "127.0.0.1";
    settings.port = 32396;
    settings.protocol = Protocol::TCP;
    Server server;
    EXPECT_NO_THROW(server.configure(settings,Socket::Option(1,Socket::Options::KeepAlive),
                        Socket::Option(1,Socket::Options::ReuseAddress)));
    auto future = std::async(std::launch::async,[&server,&settings](){
        EXPECT_NO_THROW(server.launch());
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    Client client;
    EXPECT_NO_THROW(client.connect(settings.host,settings.port));
    client.add_request(send_byte);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(server.get_connection_pool().count_recv,5);
}



class PingPong:public AbstractProcess<PingPong>{
};

class ConnectionPoolPingPong:public AbstractConnectionPool<PingPong>{
    FRIEND_TEST(Client_server,pingpong);
    int count_recv = 0;
    int count_send = 0;
    virtual void execute(std::stop_token,const Socket& socket) override;
};

void ConnectionPoolPingPong::execute(std::stop_token,const Socket& socket){
    std::string buffer;
    int tryings = 100;
    while(tryings-->=0 && count_recv<5){
        buffer.clear();
        buffer.resize(4);
        if(receive(socket,buffer,4)!=-1 && buffer=="ping"){
            std::cout<<"Receiving successed"<<std::endl;
            ++count_recv;
            if(send(socket,buffer)!=-1)
            {
                std::cout<<"Sending successed"<<std::endl;
                ++count_send;
            }
        }
        else std::cout<<"Receiving failed"<<std::endl;
    }
}

class ServerPingPong:public CommonServer<ConnectionPoolPingPong>{
    FRIEND_TEST(Client_server,pingpong);
    void after_accept(network::Socket& socket) override{
        try{
            using Event_t = Multiplexor::Event;
            std::cout<<"Connecting ";
            socket.print_address_info(std::cout);
        }
        catch(const std::runtime_error& err){
            std::cout<<"Error after accepting connection"<<std::endl;
            return;
        }
        return;
    }
};

class ClientPingPong:public CommonClient<PingPong>{
    virtual void after_connect(const Socket& sock){
        std::cout<<"Connected to: ";
        sock.print_address_info(std::cout);
    }
};

void pingpong(std::stop_token stop,const Socket& socket,int& count_send,int& count_recv){
    int tryings = 100;
    for(count_send=0;count_send<5 && tryings>=0;--tryings){
        std::string ping="ping";
        if(send(socket,ping)!=-1){
            std::cout<<"Sending successed"<<std::endl;   
            ++count_send;
            std::string pong;
            pong.resize(4);
            if(receive(socket,pong,4)!=-1 && pong=="ping"){
                std::cout<<"Receiving successed"<<std::endl;
                ++count_recv;
            }
        }
        else{
            std::cout<<"Sending failed"<<std::endl;
            continue;
        }
    }
}

TEST(Client_server,pingpong){
    server::Settings settings;
    settings.host = "127.0.0.1";
    settings.port = 32396;
    settings.protocol = Protocol::TCP;
    ServerPingPong server;
    EXPECT_NO_THROW(server.configure(settings,Socket::Option(1,Socket::Options::KeepAlive),
                        Socket::Option(1,Socket::Options::ReuseAddress)));
    auto future = std::async(std::launch::async,[&server,&settings](){
        EXPECT_NO_THROW(server.launch());
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ClientPingPong client;
    int count_send = 0;
    int count_recv = 0;
    EXPECT_NO_THROW(client.connect(settings.host,settings.port));
    client.add_request(pingpong,count_send,count_recv);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(server.get_connection_pool().count_recv,5);
    EXPECT_EQ(server.get_connection_pool().count_send,5);
    EXPECT_EQ(count_send,5);
    EXPECT_EQ(count_recv,5);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}