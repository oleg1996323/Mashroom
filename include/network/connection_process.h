#pragma once
#include <network/def.h>
#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <network/message.h>

namespace server{
    class Server;
}

using namespace server;

namespace connection{
    class Process;
    using ProcessPool = std::list<Process>;
    using ProcessInstance = std::list<Process>::iterator;
    class Process{
        private:
        std::vector<char> buffer_;
        ProcessInstance instance_;
        std::unique_ptr<std::thread> thread_;
        Server* server_;
        Socket connection_socket_;
        network::TYPE_MESSAGE msg_t_;
        Process(Server* server, int connection_socket, size_t buffer_sz=4096);
        Process(const Process&) = delete;
        void __send_error_and_close_connection__(ErrorCode err,const char* msg_err);
        void __send_error_and_continue__(ErrorCode err,const char* msg_err);
        bool __check_and_notify_if_server_inaccessible__();
        public:
        Process(Process&& other);
        ~Process();
        static Process& init(Server* server,int connection_socket, size_t buffer_sz=4096);
        void set_buffer_size(size_t sz = 4096);
        ErrorCode launch();
        ProcessInstance get_instance() const;
    };
}