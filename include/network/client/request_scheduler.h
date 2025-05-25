#pragma once
#include <queue>
#include <network/client/message.h>

namespace network::connection{
    class Scheduler{
        public:
        void add_request(network::client::MessageProcess&&);
        void add_request(const network::client::MessageProcess&);
        template<typename MSG_T,
        template
        <typename>
        typename MESSAGE>
        requires network::client::MessageType_concept<MSG_T>
        void add_request(MESSAGE<MSG_T>&& msg){
            request_seq_.push(std::forward<MESSAGE<MSG_T>>(msg));
        }
        void remove_last_request();
        private:
        std::queue<network::client::MessageProcess> request_seq_;
    };
}