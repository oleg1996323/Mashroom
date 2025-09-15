#pragma once
#include "socket.h"
#include <thread>
#include <future>
#include <stdexcept>
#include <chrono>
#include <functional>

namespace network{

template<typename DERIVED>
class AbstractProcess{
    std::jthread thread;
    std::future<void> future;
    public:
    AbstractProcess() = default;
    AbstractProcess(const AbstractProcess&) = delete;
    AbstractProcess(AbstractProcess&& other) noexcept:
    thread(std::move(other.thread)),
    future(std::move(other.future)){}
    AbstractProcess& operator=(const AbstractProcess&) = delete;
    AbstractProcess& operator=(AbstractProcess&& other) noexcept{
        if(this!=&other){
            thread = std::move(other.thread);
            future = std::move(other.future);
        }
        return *this;
    }
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
    template<typename... ARGS>
    static std::unique_ptr<DERIVED> add_process(std::function<void(std::stop_token,const Socket&,ARGS&&...)> function,const Socket& socket, ARGS&&... args){
        static_assert(std::is_base_of_v<AbstractProcess,DERIVED>,"Is not derived from AbstractProcess");
        std::unique_ptr<DERIVED> process = std::make_unique<DERIVED>();
        std::promise<void> promise;
        process->future = promise.get_future();
        process->thread =std::jthread([promise = std::move(promise),sock = socket,func = std::move(function),...arguments = std::forward<ARGS>(args)](std::stop_token stop) mutable{
            try{
                std::invoke(func,stop,sock,std::forward<decltype(arguments)>(arguments)...);
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