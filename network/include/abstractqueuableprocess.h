#pragma once
#include "abstractprocess.h"
#include <queue>
#include <mutex>

namespace network{
    class CommonQueuableProcess:public CommonProcess<size_t>{
        using Queue_task = std::function<size_t(std::stop_token)>;
        std::deque<TaskHandler<size_t>> queue;
        std::stop_source stop_;
        protected:
        virtual void after_launch() override final{
            do{
                if(!queue.empty()){
                    auto exp_task = std::move(queue.front());
                    queue.pop_front();
                    exp_task.launch(???);
                }
            }while(!stop_.stop_requested());
        }
        public:
        CommonQueuableProcess():CommonProcess<size_t>(){}
        virtual ~CommonQueuableProcess(){
            request_stop_protected(false);
        }
        CommonQueuableProcess(CommonQueuableProcess&& other) noexcept:
            CommonProcess<size_t>(std::move(other)),
            queue(std::move(other.queue)){}
        CommonQueuableProcess& operator=(CommonQueuableProcess&& other) noexcept{
            if(this!=&other){
                queue=std::move(other.queue);
                CommonProcess<size_t>::operator=(std::move(other));
            }
            return *this;
        }
        CommonQueuableProcess(const CommonQueuableProcess& other) = delete;
        CommonQueuableProcess& operator=(const CommonQueuableProcess& other) = delete;
        virtual void request_stop(bool wait_finish,uint16_t timeout_sec = 60) override final{
            queue.clear();
            if(!wait_finish){
                stop_.request_stop();
            }
            else
                queue_ready();
        }
        bool queue_ready(){
            return queue.empty() && wait(-1);
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