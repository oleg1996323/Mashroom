#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <thread>
#include <sys/eventfd.h>
#include "definitions.h"
#include "serversettings.h"
#include "concepts.h"
#include "commonsocket.h"
#include "multiplexor.h"
#include <stdexcept>
#include <set>
#include "threadpool.h"
#include <unordered_set>

namespace network{
    class CommonServer{
        std::unique_ptr<Multiplexor> accepter;
        ThreadPool processes_pool_;
        std::unordered_set<Socket> connections_pool_;
        Socket socket_;
        using Interrupted = bool;
        std::future<Interrupted> accept_result;
        void __listen_throw__(){
            int err = errno;
            errno = 0;
            throw std::runtime_error(strerror(err));
        }
        void __accept_throw__(const Socket& socket);
        public:
        using Event = Multiplexor::Event;
        protected:
        virtual void process_max_fd_reached(const Socket& socket){
            std::error_code err = std::make_error_code(
                std::errc::too_many_files_open);
            std::cout<<err.message()<<std::endl;
        }
        virtual void system_max_fd_reached(const Socket& socket){
            std::error_code err = std::make_error_code(
                std::errc::too_many_files_open_in_system);
            std::cout<<err.message()<<std::endl;
        }
        virtual void connection_aborted(const Socket& socket){
            std::error_code err = std::make_error_code(
                std::errc::connection_aborted);
            std::cout<<err.message()<<std::endl;
        }
        template<typename PROCESS>
        CommonServer& add_connection(PROCESS process){
            static_assert(std::is_base_of_v<AbstractProcess,std::decay_t<PROCESS>>);
            processes_pool_.addProcess(std::make_unique<PROCESS>(std::move(process)));
            return *this;
        }
        CommonServer& remove_connection(const Socket& socket, bool wait){
            remove_connection(socket,wait);
            return *this;
        }
        CommonServer& modify_connection(const Socket& socket,Event events_notify){
            if(auto found = connections_pool_.find(socket);found!=connections_pool_.end())
                found->(socket,events_notify);
            return *this;
        }
        public:
        template<typename... CONNPOOL_ARGS>
        CommonServer(CONNPOOL_ARGS&&... args):processes_pool_(std::forward<CONNPOOL_ARGS>(args)...){}
        template<typename... CONNPOOL_ARGS>
        CommonServer(const server::Settings& settings,CONNPOOL_ARGS&&... args):
        socket_(Socket(settings.host_,settings.port_,Socket::Type::Stream,settings.protocol_)),
        processes_pool_(std::forward<CONNPOOL_ARGS>(args)...){
            if(settings.reuse_address_)
                socket_.set_option(network::Socket::Option<int>(1,Socket::Options::ReuseAddress));
            socket_.bind();
        }
        virtual ~CommonServer(){
            if(accepter)
                accepter->interrupt();
            accept_result.wait();
        }
        template<typename... ARGS>
        void configure(const server::Settings& settings, Socket::Option<ARGS>... options){
            if(!is_launched()){
                socket_ = Socket(settings.host_,settings.port_,Socket::Type::Stream,settings.protocol_);
                socket_.set_options(std::forward<decltype(options)>(options)...);
                socket_.bind();
            }
            else{
                throw std::runtime_error("Server is not stopped for further configuration!");
            }
        }
        virtual void before_listen(){};
        void listen(int backlog){
            before_listen();
            assert(socket_.is_valid());
            if(::listen(socket_.__descriptor__(),backlog)==-1)
                __listen_throw__();
            after_listen();
        }
        virtual void after_listen(){};
        virtual void before_accept(){
            std::cout<<"server: waiting for connections…"<<std::endl;
        };
        void accept();
        virtual void after_accept(Socket& accepted){};
        const Socket socket() const{
            return socket_;
        }
        virtual void at_closing(){}
        virtual void at_collapsing(){}
        void close(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void collapse(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void launch(){
            if(!socket_.is_valid())
                throw std::runtime_error("Server not configured");
            listen(5);
            try{
                accepter = std::move(std::make_unique<Multiplexor>(5+1));
                accepter->add(socket_,Multiplexor::Event::In|Multiplexor::Event::HangUp|Multiplexor::Event::EdgeTrigger);
            }
            catch(...){
                using namespace std::string_literals;
                accepter.reset();
                throw std::runtime_error("server epoll init: "s+strerror(errno));
            }
            accept();
        }
        bool is_launched() const{
            return socket_.is_valid() && accepter;
        }
        const ThreadPool& get_connection_pool() const{
            return processes_pool_;
        }
    };
}

namespace network{
    void CommonServer::__accept_throw__(const Socket& socket){
        std::error_code err = 
            std::make_error_code(static_cast<std::errc>(errno));
        errno = 0;
        switch(static_cast<std::errc>(err.value())){
            #ifdef EAGAIN
            case std::errc::resource_unavailable_try_again:
            #elif defined EWOULDBLOCK
            case std::errc::operation_would_block:
            #endif
            case std::errc::interrupted:
                break;
            case std::errc::connection_aborted:
                connection_aborted(socket);
                break;
            case std::errc::too_many_files_open:
                process_max_fd_reached(socket);
            case std::errc::too_many_files_open_in_system:
                system_max_fd_reached(socket);
                break;
            case std::errc::operation_not_permitted:
                break;
            case std::errc::bad_file_descriptor:
            case std::errc::no_buffer_space:
            case std::errc::not_enough_memory:
            case std::errc::not_a_socket:
            case std::errc::operation_not_supported:
            case std::errc::protocol_error:
            case std::errc::bad_address:
                throw std::runtime_error(err.message());
            case std::errc::invalid_argument:
                throw std::invalid_argument(err.message());
        }
    }

    void CommonServer::accept(){
        accept_result= std::async(std::launch::async,[this](){
            socklen_t sin_size = address_struct_size(*socket_.storage);
            for(;;){
                if (accepter->interrupted()){
                    accepter.reset();
                    return true;
                }
                try{
                    assert(socket_.is_valid());
                    for(auto event: accepter->wait(-1)){
                        if(event.data.fd==socket_.__descriptor__()){
                            sockaddr_storage another;
                            memset(&another,0,sizeof(sockaddr_storage));
                            try{
                                before_accept();
                                if(int raw_sock = ::accept(socket_.__descriptor__(),(sockaddr*)&another,&sin_size);raw_sock==-1){
                                    Socket sock(raw_sock,another);
                                    #ifdef DEBUG
                                    std::cout<<"Connection refused: ";
                                    sock.print_address_info(std::cout);
                                    #endif
                                    __accept_throw__(sock);
                                    std::cout<<strerror(errno)<<std::endl;
                                    errno = 0;
                                    continue;
                                }
                                else{
                                    Socket socket(raw_sock,another);
                                    using Event_t = Multiplexor::Event;
                                    processes_pool_.add_connection(socket,Event_t::HangUp|Event_t::In);
                                    after_accept(socket);
                                    continue;
                                }
                            }
                            catch(const std::runtime_error& err){
                                std::cout<<err.what()<<std::endl;
                                continue;
                            }
                        }
                        else{
                            if(connections_pool_.contains(event.data.fd))
                                processes_pool_.get_socket(event.data.fd);
                        }
                    }
                }
                catch(const std::exception& err){
                    std::cout<<err.what()<<std::endl;
                    return false;
                }
            }
            return false;
        });
    }

    void CommonServer::close(bool wait_for_end_connections, uint16_t timeout_sec){
        if(accepter)
            accepter->interrupt();
        
        processes_pool_.stop(wait_for_end_connections,timeout_sec);
        accept_result.wait();
    }
    void CommonServer::collapse(bool wait_for_end_connections, uint16_t timeout_sec){
        //@todo Block sockets maybe?
    }
}