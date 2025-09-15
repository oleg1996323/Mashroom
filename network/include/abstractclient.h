#pragma once
#include <unistd.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <variant>
#include <netdb.h>
#include "socket.h"
#include "abstractprocess.h"
#include ""

namespace network{
    class ClientsHandler;
}
namespace network{

    template<typename PROCESS_T>
    class AbstractClient{
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
        void add_request(std::function<void(const Socket&,ARGS&&...)> function,ARGS&&... args) const{

        }
    };
}