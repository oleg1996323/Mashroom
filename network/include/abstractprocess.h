#pragma once
#include "commonsocket.h"
#include <thread>
#include <future>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <any>
#include <tuple>

namespace network{

template<typename PROCESS_T,typename RESULT_T>
class AbstractQueuableProcess;

template<typename T>
constexpr auto capture_arg(T&& v)
{
    // lvalue -> std::reference_wrapper
    if constexpr (std::is_lvalue_reference_v<T>)
        return std::reference_wrapper<std::remove_reference_t<T>>(v);
    else if constexpr(std::is_rvalue_reference_v<T>)
        return std::forward<T>(v);
    else static_assert(false,"Error at capturing argument");
}

template<typename T>
constexpr decltype(auto) unwrap_arg(T&& v)
{
    using decay = std::decay_t<T>;
    if constexpr (std::is_same_v<
                      decay,
                      std::reference_wrapper<typename decay::type>>)
        return v.get();
    else
        return std::forward<T>(v);
}

template<typename T>
concept CheckUnwrap = requires(T& i){
    {unwrap_arg(std::ref(i))}->std::same_as<T&>;
    {unwrap_arg(capture_arg(i))}->std::same_as<T&>;
};
static_assert(CheckUnwrap<int>);

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
    virtual void request_stop_protected(bool wait_finish, uint16_t timeout_sec = 60){
        if(wait_finish)
            future.wait_for(std::chrono::seconds(timeout_sec)) == std::future_status::ready;
        thread.request_stop();
    }
    public:
    virtual void action_if_process_busy(){}
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
        return AbstractProcess::__ready__();
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
        request_stop_protected(wait_finish,timeout_sec);
    }

    template<typename... CONSTR_ARGS,typename F,typename... ARGS>
    static std::unique_ptr<DERIVED> add_process(CONSTR_ARGS&&... constr_args,F&& function,const Socket& socket, ARGS&&... args){
        static_assert(std::is_base_of_v<AbstractProcess<DERIVED,RESULT_T>,DERIVED>,"Is not derived from AbstractProcess");
        static_assert(std::is_invocable_r_v<RESULT_T,std::decay_t<F>,std::stop_token,const Socket&,ARGS...>);
        std::unique_ptr<DERIVED> process = std::make_unique<DERIVED>(std::forward<CONSTR_ARGS>(constr_args)...);
        process->thread =std::jthread([ proc = process.get(),
                                        sock = socket,
                                        func = std::move(function),
                                        arguments = std::move(std::tuple(capture_arg(std::forward<ARGS>(args))...))]
                (std::stop_token stop) mutable
        {
            std::promise<RESULT_T> prom;
            proc->future = prom.get_future();
            auto body = [&](auto&&... tupArgs)
            {
                try
                {
                    if constexpr (std::is_void_v<RESULT_T>)
                    {
                        std::invoke(func, stop, sock,
                                    unwrap_arg(std::forward<decltype(tupArgs)>(tupArgs))...);
                        prom.set_value();
                    }
                    else
                    {
                        prom.set_value(
                            std::invoke(func, stop, sock,
                                        unwrap_arg(std::forward<decltype(tupArgs)>(tupArgs))...));
                    }
                }
                catch (...)
                {
                    prom.set_exception(std::current_exception());
                }
            };
            std::apply(body, std::move(arguments));
            proc->__after_execution__();
        });
        return process;
    }
};
}