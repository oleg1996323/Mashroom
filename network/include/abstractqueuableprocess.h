#pragma once
#include "abstractprocess.h"
#include <queue>
#include <mutex>

namespace network{
    template<typename PROCESS_T,typename RESULT_T = void>
    class AbstractQueuableProcess:public AbstractProcess<PROCESS_T,RESULT_T>{
        using Queue_task = std::function<RESULT_T(std::stop_token)>;
        std::deque<Queue_task> queue;
        std::condition_variable cv;
        protected:
        virtual void after_execution_protected() override final{
            std::unique_lock lk(this->m);
            do{
                if(!queue.empty()){
                    auto task = std::move(queue.front());
                    queue.pop_front();
                    lk.unlock();
                    std::invoke(task,this->thread.get_stop_token());
                    lk.lock();
                }
                cv.notify_one();
                cv.wait(lk,[this](){return !this->queue.empty() ||
                    this->thread.get_stop_token().stop_requested();});
            }while(!this->thread.get_stop_token().stop_requested());
        }
        public:
        AbstractQueuableProcess():AbstractProcess<PROCESS_T,RESULT_T>(){}
        virtual ~AbstractQueuableProcess(){
            request_stop_protected(false);
        }
        AbstractQueuableProcess(AbstractQueuableProcess&& other) noexcept:
            AbstractProcess<PROCESS_T,RESULT_T>(std::move(other)),
            queue(std::move(other.queue)){}
        AbstractQueuableProcess& operator=(AbstractQueuableProcess&& other) noexcept{
            if(this!=&other){
                std::lock_guard lk(this->m);
                queue=std::move(other.queue);
                AbstractProcess<PROCESS_T,RESULT_T>::operator=(std::move(other));
            }
            return *this;
        }
        AbstractQueuableProcess(const AbstractQueuableProcess& other) = delete;
        AbstractQueuableProcess& operator=(const AbstractQueuableProcess& other) = delete;
        virtual void request_stop_protected(bool wait_finish,uint16_t timeout_sec = 60) override final{
            std::unique_lock lk(this->m);
            lk.lock();
            queue.clear();
            if(!wait_finish)
                this->thread.request_stop();
            else{
                lk.unlock();
                cv.wait(lk,[this](){
                    return queue_ready();
                });
                return;
            }
            lk.unlock();
            return;
        }
        bool queue_ready(){
            return queue.empty() && this->ready();
        }
        template<typename F,typename... ARGS>
        void enqueue(F&& function,
                const Socket& socket,
                ARGS&&... args)
        {
            static_assert(std::is_invocable_r_v<RESULT_T,std::decay_t<F>,std::stop_token,const Socket&, ARGS...>);
            Queue_task task = [func = std::move(function),
            sock = socket,
            tup = std::move(std::tuple(capture_arg(std::forward<ARGS>(args))...)),
            promise = std::promise<RESULT_T>(),
            &future = this->future]
            (std::stop_token stop)
            {
                future = promise.get_future();
                auto body = [&](auto&&... unpacked){
                    try{
                        if constexpr (std::is_void_v<RESULT_T>){
                            std::invoke(func, stop, sock,
                                unwrap_arg(std::forward<decltype(unpacked)>(unpacked)...));
                            promise.set_value();
                        }
                        else{
                            promise.set_value(std::invoke(func, stop, sock,
                                unwrap_arg(std::forward<decltype(unpacked)>(unpacked)...)));
                        }
                    }
                    catch(...){
                        promise.set_exception(std::current_exception());
                    }
                };
                std::apply(body,std::move(tup));
            };
            std::lock_guard lock(this->m);
            queue.push_back(std::move(task));
            cv.notify_one();
        }
    };
}