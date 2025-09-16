#pragma once
#include <unistd.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <variant>
#include <netdb.h>
#include "socket.h"
#include "abstractprocess.h"

namespace network{
    class ClientsHandler;
}
namespace network{

    template<typename PROCESS_T>
    class AbstractClient{
        static_assert(std::is_base_of_v<AbstractProcess,PROCESS_T>,
            "Template argument must be derived from network::AbstractProcess class!");
        private:
        /**
         * @brief don't block the GUI if integrated
         */
        std::unique_ptr<PROCESS_T> process;
        Socket socket_;
        AbstractClient(const std::string& host, Port port):
        socket_(host,port,Socket::Type::Stream,Protocol::TCP){}
        public:
        AbstractClient(AbstractClient&& other) noexcept;
        AbstractClient& operator=(AbstractClient&& other) noexcept;
        bool operator==(const AbstractClient& other) const noexcept;
        ~AbstractClient();
        void cancel();
        virtual AbstractClient& before_connect(){return *this;}
        AbstractClient& connect(const std::string& host, const std::string& port);
        virtual AbstractClient& after_connect(const Socket&){return *this;}
        bool is_connected() const;
        virtual AbstractClient& before_disconnect(const Socket&){return *this;}
        AbstractClient& disconnect(bool wait_finish);
        virtual AbstractClient& after_disconnect(){return *this;}
        template<typename... ARGS>
        void add_request(PROCESS_T::Function_t function,ARGS&&... args) const{
            if(process && process->busy())
                process->action_if_process_busy(std::move(function),socket_,std::forward<ARGS>(args)...);
            else process = PROCESS_T::add_process(std::move(function),socket_,std::forward<ARGS>(args)...);
        }
    };
}