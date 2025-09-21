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
        std::future<void> result;
        std::mutex m;
        template<typename DERIVED_CONNECTIONPOOL>
        friend class CommonServer;
        bool contains_socket(int raw_socket){
            std::lock_guard lk(m);
            return peers_.contains(raw_socket);
        }
        const Socket& get_socket(int raw_socket){
            if(!peers_.contains(raw_socket))
                throw std::invalid_argument("Invalid file descriptor");
            return peers_.find(raw_socket)->first;
        }
        protected:
        const std::unique_ptr<PROCESS_T>& process(const Socket& socket) const{
            return peers_.at(socket);
        }
        public:
        using Event = Multiplexor::Event;
        AbstractConnectionPool():mp_connections(std::make_unique<Multiplexor>(5)){
            std::cout<<"ConnectionPool creation"<<std::endl;
        }
        virtual ~AbstractConnectionPool(){
            stop(false,0);
        }

        virtual void execute(std::stop_token,const Socket& socket) = 0;
        AbstractConnectionPool& add_connection(const Socket& socket, Event events_notify){
            std::lock_guard lk(m);
            if(!peers_.contains(socket)){
                peers_[socket];
                mp_connections->add(socket,events_notify);
            }
            std::cout<<"Added new connection"<<std::endl;
            return *this;
        }
        AbstractConnectionPool& remove_connection(const Socket& socket, bool wait){
            std::lock_guard lk(m);
            peers_.erase(socket);
            mp_connections->remove(socket);
            std::cout<<"Removed connection"<<std::endl;
            return *this;
        }
        AbstractConnectionPool& modify_connection(const Socket& socket,Event events_notify){
            std::lock_guard lk(m);
            if(peers_.contains(socket)){
                peers_[socket];
                mp_connections->modify(socket,events_notify);
            }
            else throw std::invalid_argument("Connection pool doesn't contains socket");
            std::cout<<"Modified connection"<<std::endl;
            return *this;
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
                    peers_.erase(socket);
                    break;
                case Event::Exclusive:
                    break;
                case Event::Error:
                    peers_.erase(socket);
                    //what kind of errors? How to process the errors?
            }
            return *this;
        }
        //@brief Мультиплексирует соединённые сокеты и распределяет процессы
        void polling_connections(){
            result = std::async(std::launch::async,[this](){
                std::span<network::Multiplexor::Event_t> events;
                for(;;)
                    try{
                        std::cout<<"Server multiplexor waiting"<<std::endl;
                        events = mp_connections->wait(-1);
                        if(mp_connections->interrupted()){
                            std::cout<<"ConnectionPool interrupted"<<std::endl;
                            return;
                        }
                        for(auto event:events){
                            std::cout<<"Pool events process"<<std::endl;
                            if(contains_socket(event.data.fd)){
                                auto found = peers_.find(event.data.fd);
                                if(found!=peers_.end()){
                                    if(event.events&Event::HangUp){
                                        peers_.erase(found->first);
                                        continue;
                                    }
                                    if(found->first.is_valid() && (!found->second || found->second->ready())){
                                        std::cout<<"Adding function"<<std::endl;
                                        found->second = std::move(PROCESS_T::add_process(&AbstractConnectionPool<PROCESS_T>::execute,this,found->first));
                                    }
                                }
                                else continue;   
                            }
                        }
                    }
                    catch(const std::exception& err){
                        std::cout<<err.what()<<std::endl;
                        return;
                    }
            });
        }
        void stop(bool wait_finish, uint16_t timeout_sec = 60){
            if(mp_connections)
                mp_connections->interrupt();
            result.wait();
            std::cout<<"Stop pool"<<std::endl;
            result = std::async(std::launch::async,[&wait_finish,
                                &timeout_sec,
                                &m = this->m,
                                &peers = this->peers_](){
                std::vector<std::future<void>> processes_res;
                {
                    std::lock_guard lk(m);
                    for(auto& [socket,process]:peers)
                        if(process)
                            processes_res.push_back(std::async(std::launch::async,[
                                    wait = wait_finish,
                                    timeout = timeout_sec,
                                    proc = std::move(process),
                                    &processes_res]()
                            {
                                if(proc)
                                    proc->request_stop(wait,timeout);
                            }));
                }
            });
            std::cout<<"Stop out"<<std::endl;
        }
    };
}