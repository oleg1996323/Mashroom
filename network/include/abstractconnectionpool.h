#pragma once
#include "multiplexor.h"
#include "commonsocket.h"
#include "abstractprocess.h"
#include <stdexcept>
#include <unordered_map>

namespace network{
    template<typename PROCESS_T>
    class AbstractConnectionPool{
        std::unique_ptr<Multiplexor> mp_connections;
        std::unordered_map<Socket,std::unique_ptr<PROCESS_T>,std::hash<Socket>,std::equal_to<Socket>> peers_;
        template<typename DERIVED_CONNECTIONPOOL>
        friend class CommonServer;
        bool contains_socket(int raw_socket){
            return peers_.contains(raw_socket);
        }
        const Socket& get_socket(int raw_socket){
            if(!peers_.contains(raw_socket))
                throw std::invalid_argument("Invalid file descriptor");
            return peers_.find(raw_socket)->first;
        }
        public:
        using Event = Multiplexor::Event;
        AbstractConnectionPool():mp_connections(std::make_unique<Multiplexor>(5)){}
        virtual ~AbstractConnectionPool(){}

        virtual void common_execution(std::stop_token,const Socket& socket) const = 0;
        AbstractConnectionPool& add_connection(const Socket& socket, Event events_notify){
            if(!peers_.contains(socket)){
                peers_[socket];
                mp_connections->add(socket,events_notify);
            }
            return *this;
        }
        AbstractConnectionPool& remove_connection(const Socket& socket, bool wait){
            peers_.erase(socket);
            mp_connections->remove(socket);
            return *this;
        }
        AbstractConnectionPool& modify_connection(const Socket& socket,Event events_notify){
            if(peers_.contains(socket)){
                peers_[socket];
                mp_connections->modify(socket,events_notify);
            }
            else throw std::invalid_argument("Connection pool doesn't contains socket");
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
                    events = mp_connections->wait(-1);
                    for(auto event:events){
                        if(contains_socket(event.data.fd)){
                            auto found = peers_.find(event.data.fd);
                            if(found!=peers_.end()){
                                if(found->first.is_valid() && (!found->second || found->second->ready()))
                                    found->second = std::move(PROCESS_T::add_process(std::move(&common_execution),*found->first));
                            }
                            else continue;   
                        }
                    }
                }
                catch(...){
                    return;
                }
        }
        void stop(bool wait_finish){

        }
    };
}