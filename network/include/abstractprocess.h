#pragma once
#include "socket.h"
#include <thread>
#include <future>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <any>

namespace network{

template<typename PROCESS_T,typename RESULT_T>
class AbstractQueuableProcess;

template<typename DERIVED,typename RESULT_T = void>
class AbstractProcess{
    private:
    friend class AbstractQueuableProcess<DERIVED,RESULT_T>;
    std::jthread thread;
    std::mutex m;
    std::shared_future<RESULT_T> future;
    virtual bool __ready__(){
        std::lock_guard lk(m);
        return future.valid() && future.wait_for(std::chrono::nanoseconds(1))==std::future_status::ready;
    }
    virtual void __after_execution__(){}
    protected:
    virtual void action_if_process_busy() override{}
    public:
    template<typename... ARGS>
    using Function_t = std::function<RESULT_T(std::stop_token,const Socket&,ARGS&&...)>;
    AbstractProcess() = default;
    AbstractProcess(const AbstractProcess&) = delete;
    AbstractProcess(AbstractProcess&& other) noexcept{
        *this=std::move(other);
    }
    AbstractProcess& operator=(const AbstractProcess&) = delete;
    AbstractProcess& operator=(AbstractProcess&& other) noexcept{
        if(this!=&other){
            thread = std::move(other.thread);
            future = std::move(other.future);
        }
        return *this;
    }
    ~AbstractProcess(){}
    bool ready(){
        return __ready__();
    }
    bool busy(){
        return !ready();
    }
    bool wait(int timeout_sec){
        if(timeout_sec<-1){
            future.wait();
            return true;
        }
        else return future.wait_for(std::chrono::seconds(timeout_sec))==std::future_status::ready;
    }
    virtual void request_stop(bool wait_finish, uint16_t timeout_sec = 60){
        if(wait_finish)
            future->wait_for(timeout_sec) == std::future_status::ready;
        thread.request_stop();
    }

    template<typename... ARGS>
    static std::unique_ptr<DERIVED> add_process(Function_t<ARGS...> function,const Socket& socket, ARGS&&... args){
        static_assert(std::is_base_of_v<AbstractProcess<DERIVED>,DERIVED>,"Is not derived from AbstractProcess");
        std::unique_ptr<DERIVED> process = std::make_unique<DERIVED>();
        process->thread =std::jthread([proc = process.get(),sock = socket,func = std::move(function),
                                    ...arguments = std::forward<ARGS>(args)](std::stop_token stop) mutable{
            std::promise<RESULT_T> promise;
            proc->future = promise.get_future();
            try{
                std::invoke(func,stop,sock,std::forward<decltype(arguments)>(arguments)...);
                promise.set_value();
            }
            catch(...){
                promise.set_exception(std::current_exception());
            }
            proc->__after_execution__();
        });
        return process;
    }
};
}