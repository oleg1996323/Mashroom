#include "network/server/connection_process.h"
#include "network/server.h"
#include "proc/extract.h"
#include "proc/index.h"
#include "program/mashroom.h"
#include <sys/mman.h>

using namespace network::connection;

namespace network{
    ErrorCode send_error(const Socket& socket,network::MessageProcess<network::Side::SERVER>& process,ErrorCode err, server::Status status){
        using namespace network;
        network::Message<Server_MsgT::ERROR> rep_msg;
        rep_msg.additional().err_ = ErrorCode::INVALID_ARGUMENT;
        rep_msg.additional().status_=network::server::Status::READY;
        ErrorCode errtmp = process.send_message<Server_MsgT::ERROR>(socket,std::move(rep_msg));
        return errtmp==ErrorCode::NONE?err:errtmp;
    }
}

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
        case Client_MsgT::DATA_REQUEST:{
            decltype(auto) msg = mprocess_.get_received_message<Client_MsgT::DATA_REQUEST>();
            Extract hExtract;
            auto init_h = [&hExtract](auto& form){
                if constexpr(std::is_same_v<std::decay_t<decltype(form)>,std::monostate>){
                    return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"extract form type",AT_ERROR_ACTION::CONTINUE);
                }
                else{
                    return hExtract.set_by_request(form);
                }
            };
            err_ = std::visit(init_h,msg.additional().form_);
            if(err_!=ErrorCode::NONE){
                err_ = send_error(socket,mprocess_,ErrorCode::INVALID_ARGUMENT,network::server::Status::READY);
                return;
            }
            auto curdir = (fs::temp_directory_path()/std::to_string(utc_tp::clock::now().time_since_epoch().count()));
            fs::remove_all(curdir);
            if(!fs::create_directories(curdir)){
                err_ = send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,network::server::Status::READY);
                return;
            }
            err_ = hExtract.add_in_path(curdir.c_str()); //@todo make setting temporary dir by config
            if(err_==ErrorCode::NONE)
                err_ = hExtract.execute();
            else{
                err_ = send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,network::server::Status::READY);
                return;
            }
            if(err_==ErrorCode::NONE){
                auto add = network::make_additional<Server_MsgT::DATA_REPLY_EXTRACT>();
                for(auto entry:fs::directory_iterator(curdir)){
                    if(entry.is_regular_file() && entry.path().extension()==".zip"){
                        add.file_sz_ = fs::file_size(curdir);
                        add.filename_ = entry.path().string();
                        add.status_ = server::Status::READY;
                    }
                    else{
                        err_ = send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,network::server::Status::READY);
                        return;
                    }
                    err_ = mprocess_.send_message<Server_MsgT::DATA_REPLY_EXTRACT>(socket,Message<Server_MsgT::DATA_REPLY_EXTRACT>(std::move(add)));
                    if(err_ == ErrorCode::NONE){
                        int fno = open(curdir.c_str(),O_RDONLY|O_DIRECT);
                        if(fno==-1){
                            err_ = send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,server::Status::READY);
                            return;
                        }
                        auto file_add = make_additional<Server_MsgT::DATA_REPLY_FILEPART>();
                        file_add.file_hash_ = std::hash<fs::path>()(entry.path());
                        file_add.status_=server::Status::READY;
                        //@todo set user-optional buffer size by config
                        auto ptr = (char*)mmap(nullptr,add.file_sz_,PROT_READ,MAP_PRIVATE,fno,0);
                        if(ptr==nullptr){
                            send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,server::Status::READY);
                            std::cout<<strerror(errno)<<std::endl;
                            errno = 0;
                            return;
                        }
                        if(int res = madvise(ptr,add.file_sz_,MADV_SEQUENTIAL);res==-1){
                            send_error(socket,mprocess_,ErrorCode::INTERNAL_ERROR,server::Status::READY);
                            std::cout<<strerror(errno)<<std::endl;
                            errno = 0;
                            return;
                        }
                        for(uint64_t offset=0;offset<add.file_sz_;++offset){
                            file_add.offset_=offset;
                            file_add.assign_file_segment(ptr,8912);
                            err_ = mprocess_.send_message<Server_MsgT::DATA_REPLY_FILEPART>(socket,std::move(file_add));
                        }
                        munmap(ptr,add.file_sz_);
                        fs::remove_all(curdir);
                    }
                }
            }
        }        
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