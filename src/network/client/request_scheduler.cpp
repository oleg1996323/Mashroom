#include "network/client/request_scheduler.h"

namespace network{

void connection::Scheduler::add_request(network::MessageHandler<Side::CLIENT>&& msg_h){
    request_seq_.push(std::move(msg_h));
}
void connection::Scheduler::remove_last_request(){
    request_seq_.pop();
}
}