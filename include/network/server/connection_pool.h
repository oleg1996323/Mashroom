#pragma once
#include <unordered_set>
#include <thread>
#include <mutex>
#include <network/server/connection_process.h>
#include <shared_mutex>
#include "network/common/multiplexor.h"
#include "network/common/socket.h"
namespace network::connection{
    class ConnectionPool{
        ::std::unordered_set<network::connection::Process<Server>> data_;
        std::mutex locker_;
        std::jthread thread_;
        const Server& server_;
        std::condition_variable notiifier_cv_;
        mutable std::atomic<bool> is_running_;
        using iterator = decltype(data_)::iterator;
        static void __proc_notifier__(ConnectionPool&);
        public:
        ConnectionPool(const Server& server);
        ~ConnectionPool();
        void notify_status_all() const;
        void shutdown_all();
        void shut_not_processing();
        void close_all_at_not_busy();
        void close_all();
        ErrorCode process_connection(Socket sock) const;
        network::server::Status server_status() const;
        bool is_connected(Socket socket) const;
        ConnectionPool::iterator add_connection(Socket connection_socket, size_t buffer_sz=4096);
        void update_connections();
    };

template<typename Derived>
class AbstractProcess;

class AbstractConnectionPool{
    Multiplexor mp_connections;
    std::unordered_map<Socket,std::unique_ptr<Process<Server>>,std::hash<Socket>,std::equal_to<Socket>> peers_;
    friend class ::network::AbstractServer;
    bool contains_socket(int raw_socket){
        return peers_.contains(raw_socket);
    }
    const Socket& get_socket(int raw_socket){
        if(!peers_.contains(raw_socket))
            throw std::invalid_argument("Invalid file descriptor");
        return peers_.find(socket)->first;
    }
    public:
    using Event = Multiplexor::Event;
    AbstractConnectionPool():mp_connections(5){}
    virtual ~AbstractConnectionPool(){}
    AbstractConnectionPool& add_connection(const Socket& socket, Event events_notify){
        mp_connections.add(socket,events_notify);
    }
    AbstractConnectionPool& remove_connection(const Socket& socket, bool wait){
        peers_.erase(socket);
        mp_connections.remove(socket);
    }
    virtual AbstractConnectionPool& event_process(const Socket& socket, Event events){
        switch(events){
            case Event::In:
                break;
            case Event::Out:
                break;
            case Event::WakeUp:
                break;
            case Event::OneShot:
                break;
            case Event::HighProrityIn:
                break;
            case Event::HangUp:
                peers_.erase(socket);
                break;
            case Event::CanReadButHangUp:
                break;
            case Event::Exclusive:
                break;
            case Event::Error:
                peers_.erase(socket);
                //what kind of errors? How to process the errors?
        }
    }
    //@brief Мультиплексирует соединённые сокеты и распределяет процессы
    void polling_connections(){
        std::span<network::Multiplexor::Event_t> events;
        for(;;)
            try{
                events = mp_connections.wait(-1);
                for(auto event:events){
                    if(contains_socket(event.data.fd)){
                        auto found = peers_.find(event.data.fd);
                        if(found!=peers_.end()){
                            if(found->first.is_valid() && (!found->second || found->second->ready()))
                                found->second = std::move(Process<Server>::add_process());
                        }
                        else continue;   
                    }
                }
            }
            catch(...){
                return;
            }
    }
};
}
