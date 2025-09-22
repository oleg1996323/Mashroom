#include "network/server/connection_process.h"
#include "network/server.h"
#include "proc/extract.h"
#include "proc/index.h"
#include "program/mashroom.h"

using namespace network::connection;

void Process<Server>::reply(std::stop_token stop,const Socket& socket,Process<Server>& proc){
    bool heavy = false;
    ErrorCode err_ = proc.mprocess_.receive_any_message(socket);
    auto msg_type = proc.mprocess_.received_message_type();
    if(err_!=ErrorCode::NONE || !msg_type.has_value())
        return;
    switch (msg_type.value())
    {
        case Client_MsgT::INDEX:{
            auto msg_or_error = proc.mprocess_.get_received_message<Client_MsgT::INDEX>();
            if(!msg_or_error.has_value())
                throw std::runtime_error("Invalid message");
            const network::Message<network::Client_MsgT::INDEX>& msg = msg_or_error.value().get();
            msg.additional().
            Message<Server_MsgT::DATA_REPLY_INDEX> rep_msg;
            rep_msg.additional().blocks_.push_back();
            Mashroom::instance().data().data();
            
        }
        case Client_MsgT::INDEX_REF:{
            //Index hindex
        }
        break;
        case Client_MsgT::DATA_REQUEST:
                    
        break;
        case Client_MsgT::SERVER_STATUS:

        break;
        case Client_MsgT::TRANSACTION:
            
        break;
        default:
            break;
    }
    assert(proc.busy());
}

Process<Server>::Process(Process&& other) noexcept:
    mprocess_(std::move(other.mprocess_)),
    AbstractQueuableProcess(std::move(other)){}
Process<Server>& Process<Server>::operator=(Process&& other) noexcept{
    if(this!=&other){
        mprocess_ = std::move(other.mprocess_),
        AbstractQueuableProcess::operator=(std::move(other));
    }
    return *this;
}
void Process<Server>::collapse(bool wait_finish,uint16_t timeout_sec){

}
void Process<Server>::close(bool wait_finish,uint16_t timeout_sec){
    
}