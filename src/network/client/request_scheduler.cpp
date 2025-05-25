#include <network/client/request_scheduler.h>

void network::connection::Scheduler::add_request(network::client::MessageProcess&& msg_h){
    request_seq_.push(std::move(msg_h));
}
void network::connection::Scheduler::add_request(const network::client::MessageProcess& msg_h){
    request_seq_.push(msg_h);
}
void network::connection::Scheduler::remove_last_request(){
    request_seq_.pop();
}