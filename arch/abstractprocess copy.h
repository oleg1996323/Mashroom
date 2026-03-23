#pragma once
#include "commonsocket.h"
#include "multiplexor.h"
#include <thread>
#include <future>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <any>
#include <tuple>
#include <execution>

namespace network{

class Executor{
    public:
    template<typename F>
    std::future<std::invoke_result_t<F>> 
        run(F&& function)
        {
            using Result_t=std::invoke_result_t<F>;
            return std::async(std::launch::async,
                std::packaged_task<Result_t()>(function));
        }
    template<typename F>
    std::future<std::invoke_result_t<F,
            std::stop_token>> 
        run_with_stop(F&& function,
            std::stop_token stop_token)
        {
            using Result_t=std::invoke_result_t<F,
            std::stop_token>;
            return std::async(std::launch::async,
                std::packaged_task<Result_t()>(
                    function,stop_token));
        }
};

class AbstractTaskHandler{
    AbstractTaskHandler& operator=(const AbstractTaskHandler& other) = delete;
    virtual AbstractTaskHandler& operator=(AbstractTaskHandler&& other) noexcept=0;
    virtual ~AbstractTaskHandler() = default;
    AbstractTaskHandler(AbstractTaskHandler&& other) noexcept = default;
    AbstractTaskHandler(const AbstractTaskHandler& other) = delete;
};

template<typename RESULT>
class TaskHandler:public AbstractTaskHandler{
    std::shared_future<RESULT> future_;
    std::stop_source stop_source_;
    public:
    TaskHandler() = default;
    TaskHandler(std::future<RESULT> future,std::stop_source stop_src = {}):
    future_(std::move(future)),stop_source_(stop_src){}
    TaskHandler(const TaskHandler& other) = delete;
    virtual TaskHandler(TaskHandler&& other) noexcept override{
        operator=(std::move(other));
    }
    TaskHandler& operator=(const TaskHandler& other) = delete;
    virtual TaskHandler& operator=(TaskHandler&& other) noexcept override{
        if(this!=&other){
            std::swap(future_,other.future_);
            std::swap(stop_source_,other.stop_source_);
        }
        return *this;
    }
    template<typename F>
    void launch(F&& function){
        future_ = std::move(Executor().run_with_stop
                (function,
                        stop_source_.get_token()));
    }
    std::optional<RESULT> get_result_timeout(
            uint16_t timeout_sec,
            std::error_code& err) const{
        if(future_.valid()){
            if(future_.wait_for(std::chrono::seconds(timeout_sec)) == 
            std::future_status::ready)
                return future_.get();
            else{
                err = std::make_error_code(std::errc::timed_out);
                return std::nullopt;
            }
        }
        else{
            err=std::make_error_code(std::errc::invalid_argument);
            return std::nullopt;
        }
    }
    ~TaskHandler(){}
    bool is_ready() const{
        return future_.valid() &&
            future_.wait_for(std::chrono::nanoseconds())==
            std::future_status::ready;
    }
    bool is_busy() const{
        return !is_ready();
    }
    bool wait(int timeout_sec){
        if(timeout_sec<0){
            future_.wait();
            return true;
        }
        else 
            return future_.wait_for(std::chrono::seconds(timeout_sec))==std::future_status::ready;
    }
    void request_stop(bool wait_finish, uint16_t timeout_sec = 60){
        if(wait_finish)
            future_.wait_for(std::chrono::seconds(timeout_sec)) ==
                std::future_status::ready;
        stop_source_.request_stop();
    }
};

class AbstractProcess{
    private:
    Executor& executor_;
    Socket socket_;
    public:
    virtual void before_launch() = 0;
    virtual void after_launch() = 0;
    template<typename F,typename... ARGS>
    void start(F&& function){
        socket_ = socket;
        before_launch();
        task_.emplace(std::move(executor_.run(function,
            socket,std::forward<ARGS>(args)...)));
        after_launch();
    }
    void start_with_stop(F&& function,
            std::stop_source stop,
            const Socket& socket,
            ARGS&&... args){
        socket_ = socket;
        before_launch();
        task_.emplace(std::move(executor_.run_with_stop(function,
            stop,socket,std::forward<ARGS>(args)...)));
        after_launch();
    }
    virtual void stop()=0;
    Socket socket() const{
        return socket_;
    }
    virtual void handleEvent(Multiplexor::Event event) const=0;
    bool is_ready() const{
        return task_ && task_.is_ready();
    }
    bool is_busy() const{
        return task_ && task_.is_busy();
    }
    bool has_task() const{
        return task_?true:false;
    }
    virtual void request_stop(bool wait_finish, uint16_t timeout_sec = 60){
        if(task_)
            task_->request_stop(wait_finish,timeout_sec);
    }
    std::optional<RESULT> get_result(int timeout_sec,std::error_code& err) const{
        if(task_){
            auto result = task_->get_result_timeout(timeout_sec,err);
            if(result.has_value())
                return *result;
            else return std::nullopt;
        }
        else{
            err=std::errc::no_message;
            return std::nullopt;
        }
    }
    CommonProcess() = default;
    CommonProcess(const CommonProcess&) = delete;
    CommonProcess(CommonProcess&& other) noexcept{
        *this=std::move(other);
    }
    CommonProcess& operator=(const CommonProcess&) = delete;
    CommonProcess& operator=(CommonProcess&& other) noexcept{
        if(this!=&other)
            task_ = std::move(other.task_);
        return *this;
    }
    virtual ~CommonProcess(){}
    bool wait(int timeout_sec){
        if(task_)
            task_->wait(timeout_sec);
    }
};

template<typename RESULT>
class CommonProcess{
    private:
    friend class AbstractQueuableProcess;
    std::optional<TaskHandler<RESULT>> task_;
    Executor& executor_;
    Socket socket_;
    public:
    virtual void before_launch(){}
    virtual void after_launch(){}
    template<typename F,typename... ARGS>
    void start(F&& function,
            const Socket& socket,
            ARGS&&... args){
        socket_ = socket;
        before_launch();
        task_.emplace(std::move(executor_.run(function,
            socket,std::forward<ARGS>(args)...)));
        after_launch();
    }
    void start_with_stop(F&& function,
            std::stop_source stop,
            const Socket& socket,
            ARGS&&... args){
        socket_ = socket;
        before_launch();
        task_.emplace(std::move(executor_.run_with_stop(function,
            stop,socket,std::forward<ARGS>(args)...)));
        after_launch();
    }
    virtual void stop(){
        if(task_)
            task_->request_stop(false,0);
    }
    Socket socket() const{
        return socket_;
    }
    virtual void handleEvent(Multiplexor::Event event) const{
        switch (event)
        {
        case Multiplexor::Event::HangUp:
        case Multiplexor::Event::CanReadButHangUp:
        case Multiplexor::Event::Error:
            request_stop(false,0);
            break;
        default:
            break;
        }
    }
    bool is_ready() const{
        return task_ && task_.is_ready();
    }
    bool is_busy() const{
        return task_ && task_.is_busy();
    }
    bool has_task() const{
        return task_?true:false;
    }
    virtual void request_stop(bool wait_finish, uint16_t timeout_sec = 60){
        if(task_)
            task_->request_stop(wait_finish,timeout_sec);
    }
    std::optional<RESULT> get_result(int timeout_sec,std::error_code& err) const{
        if(task_){
            auto result = task_->get_result_timeout(timeout_sec,err);
            if(result.has_value())
                return *result;
            else return std::nullopt;
        }
        else{
            err=std::errc::no_message;
            return std::nullopt;
        }
    }
    CommonProcess() = default;
    CommonProcess(const CommonProcess&) = delete;
    CommonProcess(CommonProcess&& other) noexcept{
        *this=std::move(other);
    }
    CommonProcess& operator=(const CommonProcess&) = delete;
    CommonProcess& operator=(CommonProcess&& other) noexcept{
        if(this!=&other)
            task_ = std::move(other.task_);
        return *this;
    }
    virtual ~CommonProcess(){}
    bool wait(int timeout_sec){
        if(task_)
            task_->wait(timeout_sec);
    }
};
}