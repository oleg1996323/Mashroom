#pragma once
#include "abstractprocess.h"
#include <queue>
#include <mutex>

namespace network{
    template<typename PROCESS_T,typename RESULT_T = void>
    class AbstractQueuableProcess:public AbstractProcess<PROCESS_T,RESULT_T>{
        using Queue_task = std::function<RESULT_T(std::stop_token)>;
        std::queue<Queue_task> queue;
        std::condition_variable cv;
        virtual void __after_execution__() override final{
            std::unique_lock lk(this->m);
            do{
                if(!queue.empty()){
                    auto task = std::move(queue.front());
                    queue.pop();
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
        virtual ~AbstractQueuableProcess() = default;
        virtual void request_stop_protected(bool wait_finish,uint16_t timeout_sec = 60) override final{
            std::unique_lock lk(this->m);
            lk.lock();
            queue.c.clear();
            if(!wait_finish)
                this->thread.request_stop();
            else{
                lk.unlock();
                cv.wait(lk,[this](){
                    return queue_ready();
                });
            }
        }
        bool queue_ready(){
            return queue.empty() && this->ready();
        }
        template<typename... ARGS>
        void enqueue(typename AbstractProcess<PROCESS_T,RESULT_T>::Function_t<ARGS...> function,
                const Socket& socket,
                ARGS&&... args)
        {
            Queue_task task = [func = std::move(function),
            sock = socket,
            tup = std::make_tuple(std::forward<ARGS>(args)...),
            promise = std::promise<RESULT_T>(),
            &future = this->future]
            (std::stop_token stop)
            {
                future = promise.get_future();
                try{
                    std::apply([&](auto&&... unpacked)
                    {
                        std::invoke(func, stop, sock,
                            std::forward<decltype(unpacked)>(unpacked)...);
                    }, tup);
                    promise.set_value();
                }
                catch(...){
                    promise.set_exception(std::current_exception());
                }
            };
            std::lock_guard lock(this->m);
            queue.push(std::move(task));
            cv.notify_one();
        }
    };
}