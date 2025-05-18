#pragma once
#include <network/common/def.h>
#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <network/server/message.h>
#include <network/client/message.h>
#include <queue>
#include <shared_mutex>

namespace server{
    class Server;
}

using namespace server;

namespace connection{
    class ConnectionPool;
    class Process{
        friend struct std::hash<connection::Process>;
        friend struct std::less<connection::Process>;
        friend struct std::equal_to<connection::Process>;
        friend class ConnectionPool;
        private:
        mutable std::vector<char> buffer_; //24 byte
        mutable std::jthread thread_; //16 byte
        const ConnectionPool& pool_; //8 byte
        Socket connection_socket_; //4 byte
        mutable std::atomic<bool> busy_ = false; //1 byte
        mutable std::atomic<bool> translating_ = false; //1 byte
        Process(const Process&) = delete;
        Process& operator=(Process&& other) = delete;   //thread_ may be launched and 
                                                        //containing old "other" pointer
        Process& operator=(const Process& other) = delete;
        void __send_error_and_close_connection__(ErrorCode err,const char* msg_err) const;
        void __send_error_and_continue__(ErrorCode err,const char* msg_err) const;
        bool __check_and_notify_if_server_inaccessible__() const;
        ErrorCode __execute_heavy_process__(client::TYPE_MESSAGE msg_t) const; //
        ErrorCode __execute_light_process__(client::TYPE_MESSAGE msg_t) const; //asyncronously
        ErrorCode __read_rest_data_at_error__(client::TYPE_MESSAGE msg) const;
        public:
        Process(int connection_socket,const ConnectionPool& pool, size_t buffer_sz=4096);
        Process(Process&& other);
        ~Process();
        ErrorCode send_status_message(server::Status status) const;
        void set_buffer_size(size_t sz = 4096);
        ErrorCode execute() const;
        bool busy() const{
            return busy_;
        }
        bool translate() const{
            return translating_;
        }
        int socket() const;
        bool is_connected() const;
        void shut() const;
        void shut_at_done() const;
        void close() const;
        void close_at_done() const;
    };
}

template<>
struct std::hash<connection::Process>{
    using is_transparent = std::true_type;
    size_t operator()(const connection::Process& process) const{
        return std::hash<size_t>{}(static_cast<size_t>(process.connection_socket_));
    }
    size_t operator()(Socket sock) const{
        return std::hash<Socket>{}(sock);
    }
};

template<>
struct std::less<connection::Process>{
    using is_transparent = std::true_type;
    size_t operator()(const connection::Process& lhs,const connection::Process& rhs) const{
        return lhs.connection_socket_<rhs.connection_socket_;
    }
};

template<>
struct std::equal_to<connection::Process>{
    using is_transparent = std::true_type;
    size_t operator()(const connection::Process& lhs,const connection::Process& rhs) const{
        return lhs.connection_socket_==rhs.connection_socket_;
    }
    size_t operator()(const connection::Process& lhs,Socket sock) const{
        return lhs.connection_socket_==sock;
    }
    size_t operator()(Socket sock,const connection::Process& rhs) const{
        return rhs.connection_socket_==sock;
    }
};