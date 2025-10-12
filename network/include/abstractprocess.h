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
constexpr decltype(auto) capture_arg(T&& v);

template<typename T>
constexpr decltype(auto) unwrap_arg(T&& v);

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
    protected:
    virtual void after_execution_protected(){}
    virtual void request_stop_protected(bool wait_finish, uint16_t timeout_sec = 60){
        if(wait_finish)
            future.wait_for(std::chrono::seconds(timeout_sec)) == std::future_status::ready;
        thread.request_stop();
    }
    public:
    void after_execution(){
        return after_execution_protected();
    }
    virtual void action_if_process_busy(){}
    AbstractProcess() = default;
    AbstractProcess(const AbstractProcess&) = delete;
    AbstractProcess(AbstractProcess&& other) noexcept{
        *this=std::move(other);
    }
    AbstractProcess& operator=(const AbstractProcess&) = delete;
    AbstractProcess& operator=(AbstractProcess&& other) noexcept{
        std::lock_guard lk(this->m);
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
        if(timeout_sec<0){
            if(thread.joinable())
                thread.join();
            future.wait();
            return true;
        }
        else {
            auto ready = (future.wait_for(std::chrono::seconds(timeout_sec))==std::future_status::ready);
            if(!ready)
                thread.join();
            return ready;
        }
    }
    virtual void request_stop(bool wait_finish, uint16_t timeout_sec = 60){
        request_stop_protected(wait_finish,timeout_sec);
    }

    template<typename... CONSTR_ARGS>
    static std::unique_ptr<DERIVED> make_process(CONSTR_ARGS&&... constr_args);
    template<typename F,typename CLASS,typename... ARGS>
    static void execute_process(const std::unique_ptr<DERIVED>& process,F&& function,CLASS class_ptr,const Socket& socket, ARGS&&... args);
    template<typename F,typename... ARGS>
    static void execute_process(const std::unique_ptr<DERIVED>& process,F&& function,const Socket& socket, ARGS&&... args);
};
}


namespace network{
template<typename T>
constexpr decltype(auto) capture_arg(T&& v)
{
    // lvalue -> std::reference_wrapper
    if constexpr (std::is_lvalue_reference_v<T>)
        return std::reference_wrapper<std::remove_reference_t<T>>(v);
    else return std::forward<T>(v);
}

// template<typename T>
// struct IsReferenceWrapper:std::false_type{};

// template<typename T>
// struct IsReferenceWrapper:std::true_type{
//     using decay = std::decay_t<T>
//     typename T::type;
//     static_assert(std::is_same_v<
//                       decay,
//                       std::reference_wrapper<typename decay::type>>);
// };

template<typename T>
concept is_reference_wrapper = requires (const T&){
    std::is_same<
    std::decay_t<T>,
    std::reference_wrapper<typename std::decay_t<T>::type>>::value;
};

template<typename T>
constexpr decltype(auto) unwrap_arg(T&& v)
{
    if constexpr (is_reference_wrapper<T>){
        return v.get();
    }
    else
        return std::forward<T>(v);
}

template<typename DERIVED, typename RESULT_T>
template<typename F,typename... ARGS>
void AbstractProcess<DERIVED,RESULT_T>::execute_process(const std::unique_ptr<DERIVED>& process,F&& function,const Socket& socket, ARGS&&... args){
    std::promise<RESULT_T> promise;
    process->future = promise.get_future();
    process->thread =std::jthread([ proc = process.get(),
                                    sock = socket,
                                    func = std::move(function),
                                    prom = std::move(promise),
                                    arguments = std::move(std::tuple(capture_arg(std::forward<ARGS>(args))...))]
            (std::stop_token stop) mutable
    {
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
        proc->after_execution();
    });
}

template<typename DERIVED, typename RESULT_T>
template<typename F,typename CLASS,typename... ARGS>
void AbstractProcess<DERIVED,RESULT_T>::execute_process(const std::unique_ptr<DERIVED>& process,F&& function,CLASS class_ptr,const Socket& socket, ARGS&&... args){
    static_assert(std::is_base_of_v<AbstractProcess<DERIVED,RESULT_T>,DERIVED>,"Is not derived from AbstractProcess");
    static_assert(std::is_pointer_v<CLASS>);
    static_assert(std::is_invocable_r_v<RESULT_T,std::decay_t<F>,CLASS,std::stop_token,const Socket&,ARGS...>);
    std::promise<RESULT_T> promise;
    process->future = promise.get_future();
    process->thread =std::jthread([ proc = process.get(),
                                    sock = socket,
                                    func = std::move(function),
                                    prom = std::move(promise),
                                    ptr = class_ptr,
                                    arguments = std::move(std::tuple(capture_arg(std::forward<ARGS>(args))...))]
            (std::stop_token stop) mutable
    {
        auto body = [&](auto&&... tupArgs)
        {
            try
            {
                if constexpr (std::is_void_v<RESULT_T>)
                {
                    std::invoke(func, ptr, stop, sock,
                                unwrap_arg(std::forward<decltype(tupArgs)>(tupArgs))...);
                    prom.set_value();
                }
                else
                {
                    prom.set_value(
                        std::invoke(func, ptr, stop, sock,
                                    unwrap_arg(std::forward<decltype(tupArgs)>(tupArgs))...));
                }
            }
            catch (...)
            {
                prom.set_exception(std::current_exception());
            }
        };
        std::apply(body, std::move(arguments));
        proc->after_execution();
    });
}

template<typename DERIVED, typename RESULT_T>
template<typename... CONSTR_ARGS>
std::unique_ptr<DERIVED> AbstractProcess<DERIVED,RESULT_T>::make_process(CONSTR_ARGS&&... constr_args){
    static_assert(std::is_base_of_v<AbstractProcess<DERIVED,RESULT_T>,DERIVED>,"Is not derived from AbstractProcess");
    std::unique_ptr<DERIVED> process = std::make_unique<DERIVED>(std::forward<CONSTR_ARGS>(constr_args)...);
    return process;
}

template<typename T>
concept CheckUnwrap = requires(T& i){
    {unwrap_arg(std::ref(i))}->std::same_as<T&>;
    {unwrap_arg(capture_arg(i))}->std::same_as<T&>;
};
static_assert(CheckUnwrap<int>);
}