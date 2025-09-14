#pragma once
#include "socket.h"
#include <thread>
#include <future>
#include <stdexcept>
#include <chrono>

namespace network{

template<typename DERIVED>
    class AbstractProcess{
        std::jthread thread;
        std::future<void> future;
        public:
        AbstractProcess(){}
        ~AbstractProcess(){}
        virtual void execute(const std::stop_token& st,const Socket& socket) = 0;
        bool ready(){
            return future.valid() && future.wait_for(std::chrono::nanoseconds(0))==std::future_status::ready;
        }
        bool busy(){
            return !ready();
        }
        bool request_stop(){
            return thread.request_stop();
        }
        static std::unique_ptr<DERIVED> add_process(){
            static_assert(std::is_base_of_v<AbstractProcess,DERIVED>,"Is not derived from AbstractProcess");
            std::unique_ptr<DERIVED> process = std::make_unique<DERIVED>();
            std::promise<void> promise;
            process->future = promise.get_future();
            process->thread =std::jthread([proc = process.get(),promise = std::move(promise)](std::stop_token stop) mutable{
                try{
                    proc->execute(stop);
                    promise.set_value();
                }
                catch(...){
                    promise.set_exception(std::current_exception());
                }
            });
            return process;
        }
    };
}