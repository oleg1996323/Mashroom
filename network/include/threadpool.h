#pragma once
#include <thread>
#include <vector>
#include <unordered_map>
#include "abstractprocess.h"
#include "multiplexor.h"
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

namespace network{

class AbstractWorker{
    public:
    AbstractWorker(uint32_t order_lenght):
    multiplexor_(order_lenght){}
    virtual void start() = 0;
    virtual void stop(bool wait_for_end_connections, uint16_t timeout_sec) = 0;
    virtual bool addProcess(std::unique_ptr<AbstractProcess> proc,
            std::error_code& err) =0;
    virtual bool removeProcess(Socket socket,
            bool wait_for_end_connections,
            uint16_t timeout_sec,
            std::error_code& err) = 0;
    bool modifyProcessEvents(Socket socket,
            Multiplexor::Event events,
            std::error_code& err){
        if(contains_connection(socket)){
            multiplexor_.modify(socket,events);
            err = std::error_code();
            return true;
        }
        else{
            err = std::make_error_code(std::errc::not_connected);
            return false;
        }
    }
    virtual bool contains_connection(Socket socket) const = 0;
protected:
    Multiplexor multiplexor_;
private:
    virtual void run(std::stop_token st) = 0;
    virtual void handlePending() = 0;
};

class Worker:public AbstractWorker{
public:
    Worker(uint32_t order_length) :
    AbstractWorker(order_length){}
    virtual void start() override{
        thread_ = std::jthread([this](std::stop_token st) { run(st); });
    }
    virtual void stop(bool wait_for_end_connections, uint16_t timeout_sec) override{
        stop_ = true;{
            std::lock
            for(auto& [sock,proc]:processes_)
                proc->request_stop(wait_for_end_connections,timeout_sec);
        }
        multiplexor_.interrupt();
        if (thread_.joinable()) thread_.join();
    }
    virtual bool addProcess(std::unique_ptr<AbstractProcess> proc,std::error_code& err) override{
        if(!stop_){
            Socket fd = proc->socket();
            {
                std::lock_guard lock(cmd_mutex_);
                pending_add_.emplace_back(std::move(proc), fd);
            }
            multiplexor_.interrupt();
            return true;
        }
        else{
            err = std::make_error_code(std::errc::interrupted);
            return false;
        }
    }
    virtual bool removeProcess(Socket socket,
            bool wait_for_end_connections,
            uint16_t timeout_sec,
            std::error_code& err) override{
        if(this->contains_connection(socket)){
            err = std::error_code();
            {
                std::lock_guard lock(cmd_mutex_);
                pending_remove_.push_back(socket);
            }
            multiplexor_.interrupt();
            return true;
        }
        else{
            err = std::make_error_code(std::errc::not_connected);
            return false;
        }
    }
private:
    void run(std::stop_token st) {
        while (!stop_ && !st.stop_requested()) {
            auto events = multiplexor_.wait(-1); // бесконечное ожидание
            if (multiplexor_.interrupted()) {
                handlePending();
            }
            for (const auto& ev : events) {
                // В вашем Multiplexor::Event_t — это epoll_event, где data.fd — это fd сокета.
                int fd = ev.data.fd;
                Multiplexor::Event e = static_cast<
                    Multiplexor::Event>(ev.events);
                auto it = processes_.find(fd);
                if (it != processes_.end()) {
                    it->second->handleEvent(e);
                    if (it->second->is_ready()) {
                        // Процесс завершился, удаляем из epoll и контейнера
                        multiplexor_.remove(it->first); // предполагаем, что есть конструктор Socket из fd
                        processes_.erase(it);
                    }
                }
            }
        }
    }

    void handlePending() {
        std::vector<std::pair<std::unique_ptr<AbstractProcess>, Socket>> adds;
        std::vector<Socket> removes;
        {
            std::lock_guard lock(cmd_mutex_);
            adds.swap(pending_add_);
            removes.swap(pending_remove_);
        }
        // Удаляем процессы
        for (Socket fd : removes) {
            auto it = processes_.find(fd);
            if (it != processes_.end()) {
                multiplexor_.remove(it->first);
                processes_.erase(it);
            }
        }
        // Добавляем новые
        for (auto& p : adds) {
            auto socket = p.second;
            auto& proc = p.first;
            // Определяем начальные события (например, EPOLLIN)
            // В вашем Multiplexor::Event — это enum с флагами.
            Multiplexor::Event events = Multiplexor::In | Multiplexor::EdgeTrigger; // например
            multiplexor_.add(socket, events);
            processes_[socket] = std::move(proc);
        }
    }
    std::jthread thread_;
    std::atomic<bool> stop_{false};
    std::unordered_map<Socket, std::unique_ptr<AbstractProcess>> processes_;

    std::mutex cmd_mutex_;
    std::vector<std::pair<std::unique_ptr<AbstractProcess>, Socket>> pending_add_;
    std::vector<Socket> pending_remove_;
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back(std::make_unique<Worker>());
            workers_.back()->start();
        }
    }

    ~ThreadPool() {
        for (auto& w : workers_) w->stop(false,0);
    }
    
    void addProcess(std::unique_ptr<AbstractProcess> proc, std::error_code& err) {
        size_t index = next_worker_++ % workers_.size();
        workers_[index]->addProcess(std::move(proc),err);
    }

    void removeProcess(Socket fd,std::error_code& err) {
        for (auto& w : workers_) {
            if(w->removeProcess(fd,false,0,err))
                break;
        }
    }
    void modifyProcessEvents(Socket socket,
                Multiplexor::Event events,
                std::error_code& err){
        for(auto& worker:workers_)
            if(worker->modifyProcessEvents(socket,events,err))
                break;
    }
    void stop_connection(Socket socket,
                        bool wait_for_end_connections,
                        uint16_t timeout_sec,
                        std::error_code& err){
        for (auto& w : workers_) 
            if(w->removeProcess(socket,
                wait_for_end_connections,
                timeout_sec,
                err))
                break;
    }
    void stop(bool wait_for_end_connections, uint16_t timeout_sec){
        for (auto& w : workers_) w->stop(wait_for_end_connections,timeout_sec);
    }
    void collapse(){
        
    }

private:
    std::vector<std::unique_ptr<Worker>> workers_;
    std::atomic<size_t> next_worker_{0};
};
}