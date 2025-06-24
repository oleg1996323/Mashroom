#pragma once
#include <queue>
#include "network/common/message/message_process.h"

namespace network::connection{
    class Scheduler{
        public:
        void add_request(network::MessageProcess<Side::CLIENT>&&);
        void add_request(const network::MessageProcess<Side::CLIENT>&);

        template <auto MSG_T>
        requires ClientMessageEnumConcept<MSG_T>
        void add_request(Message<MSG_T>&& msg){
            request_seq_.emplace(std::forward<Message<MSG_T>>(msg));
        }
        void remove_last_request();
        private:
        std::queue<network::MessageProcess<Side::CLIENT>> request_seq_;
    };
}