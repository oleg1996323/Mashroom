#include "network/server/connection_process.h"
#include "network/server.h"
#include "proc/extract.h"
#include "proc/index.h"
#include "program/mashroom.h"

using namespace network::connection;

void Process<Server>::reply(std::stop_token stop,const Socket& socket){
    bool heavy = false;
    ErrorCode err_ = mprocess_.receive_any_message(socket);
    auto msg_type = mprocess_.received_message_type();
    if(err_!=ErrorCode::NONE || !msg_type.has_value())
        return;
    switch (msg_type.value())
    {
        case Client_MsgT::INDEX_REF:
        case Client_MsgT::INDEX:{
            decltype(auto) msg = mprocess_.get_received_message<Client_MsgT::INDEX_REF>();
            Message<Server_MsgT::DATA_REPLY_INDEX_REF> rep_msg;
            auto find_data = [&msg,&rep_msg](const auto& index_param) mutable ->void
            {
                if constexpr (std::is_same_v<std::decay_t<decltype(index_param)>,std::monostate>)
                    return;
                else if constexpr (std::is_same_v<std::decay_t<decltype(index_param)>,IndexParameters<Data_t::METEO,Data_f::GRIB>>){
                    auto result = Mashroom::instance().data().find_all<Data_t::METEO,Data_f::GRIB>(index_param.grid_type_,index_param.time_,index_param.forecast_preference_,msg.additional().last_update_);
                    if(!result.empty())
                        //@todo add access mode
                        rep_msg.additional().add_block<Data_t::METEO,Data_f::GRIB>(Data_a::PUBLIC,std::move(result));
                    else return;
                }
                else static_assert(false);
            };
            for(auto& param : msg.additional().parameters_) 
                std::visit (find_data,param);
            mprocess_.send_message<Server_MsgT::DATA_REPLY_INDEX_REF>(socket,std::move(rep_msg));
            //if(msg_type.value()==Client_MsgT::INDEX)
                //sendfile
            //@todo send file
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
    assert(busy());
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