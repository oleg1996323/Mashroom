#pragma once
#include "abstractprocess.h"
#include <queue>
#include <mutex>

namespace network{
    template<typename PROCESS_T>
    class AbstractQueuableProcess:public AbstractProcess<PROCESS_T>{
        using Queue_task = std::function<void(std::stop_token)>;
        std::queue<Queue_task> queue;
        std::condition_variable cv;
        virtual void __after_execution__() override final{
            std::unique_lock lk(m);
            do{
                if(!queue.empty()){
                    auto task = std::move(queue.front());
                    queue.pop();
                    lk.unlock();
                    std::invoke(task,thread.get_stop_token());
                    lk.lock();
                }
                cv.notify_one();
                cv.wait(lk,[this](){return !this->queue.empty() ||
                    this->thread.get_stop_token().stop_requested();});
            }while(!thread.get_stop_token().stop_requested());
        }
        public:
        ~AbstractQueuableProcess() = default;
        void request_stop(bool wait_finish){
            std::unique_lock lk(m);
            lk.lock();
            queue.c.clear();
            if(!wait_finish)
                request_stop();
            else{
                lk.unlock();
                cv.wait(lk,[this](){
                    return queue_ready();
                });
            }
        }
        bool queue_ready(){
            return queue.empty() && __ready__();
        }
        template<typename... ARGS>
        void enqueue(Function_t<ARGS> function,
                const Socket& socket,
                ARGS&&... args)
        {
            Queue_task task = [func = std::move(function),
            sock = socket,
            tup = std::make_tuple(std::forward<ARGS>(args)...),
            promise = std::promise<void>(),
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
            std::lock_guard lock(m);
            queue.push(std::move(task));
            cv.notify_one();
        }
    };
}