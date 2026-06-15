#include "proc/extract.h"
#include "proc/index.h"
#include "network/abstractprocess.h"
#include "web/server/connection_process.h"
#include "program/mashroom.h"

using namespace network;

template <Data_t TYPE,Data_f FORMAT>
void 
    find_data(
        const network::Message<Client_MsgT::INDEX_REF>& input,
        network::Message<network::Server_MsgT::INDEX>& output,
        const IndexParameters<TYPE,FORMAT>& index_param)
{
    if constexpr (TYPE == Data_t::TIME_SERIES && FORMAT == Data_f::GRIB_v1){
        network::Message<network::Server_MsgT::INDEX> rep_input;
        auto result = Mashroom::instance().data().find_all<TYPE,FORMAT>(
            index_param.common_,
            input.last_update_,
            index_param.top_,index_param.bottom_,
            index_param.left_,index_param.right_,
            index_param.from_,
            index_param.to_,
            index_param.tdiff_,
            index_param.forecast_preference_,
            index_param.level_,
            index_param.grid_type_);
        if(!result.empty())
            //@todo add access mode
            output.add_block<TYPE,FORMAT>(Data_a::PUBLIC,std::move(result));
        else return;
    }
    else static_assert(false);
};

template <Data_t TYPE,Data_f FORMAT>
void 
    find_data(
        const network::Message<Client_MsgT::INDEX>& input,
        network::Message<network::Server_MsgT::INDEX>& output,
        const IndexParameters<TYPE,FORMAT>& index_param)
{
    if constexpr (TYPE == Data_t::TIME_SERIES && FORMAT == Data_f::GRIB_v1){
        network::Message<network::Server_MsgT::INDEX> rep_input;
        auto result = Mashroom::instance().data().find_all<TYPE,FORMAT>(
            index_param.common_,
            input.last_update_,
            index_param.top_,index_param.bottom_,
            index_param.left_,index_param.right_,
            index_param.from_,
            index_param.to_,
            index_param.tdiff_,
            index_param.forecast_preference_,
            index_param.level_,
            index_param.grid_type_);
        if(!result.empty())
            //@todo add access mode
            output.add_block<TYPE,FORMAT>(Data_a::PUBLIC,std::move(result));
        else return;
    }
    else static_assert(false);
};

network::Message<Server_MsgT::INDEX> index_ref_process(
        std::error_code& err,
        std::stop_token token,
        const network::Message<Client_MsgT::INDEX_REF>& msg){
    const auto& transaction = msg.transaction().get_reply();
    network::Message<network::Server_MsgT::INDEX> rep_msg(std::move(transaction));
    auto find_data_proxy = [&](const auto& val){
        if constexpr (std::is_same_v<std::monostate,std::decay_t<decltype(val)>>)
            return;
        else find_data(msg,rep_msg,val);
    };
    for(auto& param : msg.parameters_) 
        std::visit(find_data_proxy,param);
    return rep_msg;
}

::Message<Server_MsgT::INDEX> index_process(std::error_code& err,
        std::stop_token token,
        const Message<Client_MsgT::INDEX>& msg)
{
    ::Message<Server_MsgT::INDEX> rep_msg;
    auto find_data_proxy = [&](const auto& val){
        if constexpr (!std::is_same_v<std::monostate,std::decay_t<decltype(val)>>)
            find_data(msg,rep_msg,val);
        else return;
    };
    for(auto& param : msg.parameters_) 
        std::visit(find_data_proxy,param);
    return rep_msg;
}

void reply(std::stop_token stop){
    bool heavy = false;
    std::error_code err;
    proc->receive_message(socket,err);
    if(err!=std::error_code()){
            send_error(socket,proc,
            ErrorCode::RECEIVING_MESSAGE_ERROR,
            server::Status::READY,
            err);
        return;
    }
    auto msg = proc->get_received_msg().lock();
    auto msg_type = proc->received_message_type(
            msg);
    if(!msg_type.has_value())
        return;

    // auto process_msg = [](const auto& msg){
    //     if constexpr(std::)
    // };
    // std::visit(process_msg,*msg);
    switch (msg_type.value())
    {
        case Client_MsgT::INDEX_REF:
            index_ref_process(
                    err,
                    stop,
                    std::get<Message<Client_MsgT::INDEX_REF>>(*msg));
            break;
        case Client_MsgT::INDEX:{
            index_process(
                    err,
                    stop,
                    std::get<Message<Client_MsgT::INDEX>>(*msg));
        }
            break;
        case Client_MsgT::EXTRACT:{
            Extract hExtract;
            auto init_h = [&hExtract](auto& form){
                if constexpr(std::is_same_v<std::decay_t<decltype(form)>,std::monostate>){
                    return ErrorPrint::print_error(
                        ErrorCode::UNDEFINED_VALUE,
                        "extract form type",
                        AT_ERROR_ACTION::CONTINUE);
                }
                else{
                    return hExtract.set_by_request(form);
                }
            };
            std::visit(init_h,
                std::get<Message<
                Client_MsgT::EXTRACT>>(*msg).additional().form_);
            if(err!=std::error_code()){
                send_error(
                    socket,
                    proc,
                    ErrorCode::INVALID_ARGUMENT,
                    network::server::Status::READY,
                    err);
                return;
            }
            std::string file_namebase = std::to_string(
                utc_tp::clock::now().time_since_epoch().count())+"_"+
                socket.ip_to_text()+":"+socket.port_to_text();
            //main directory of current request with .zip file and cache-dir
            auto curdir = fs::temp_directory_path()/file_namebase;
            fs::remove_all(curdir);
            if(!fs::create_directories(curdir)){
                send_error(
                        socket,
                        proc,
                        ErrorCode::INTERNAL_ERROR,
                        network::server::Status::READY,
                        err);
                return;
            }
            //@todo change further to std::error_code
            ErrorCode error_code;
            //@todo make setting temporary dir by config
            error_code = hExtract.add_in_path(curdir.c_str()); 
            if(error_code==ErrorCode::NONE)
                error_code = hExtract.execute();
            else{
                send_error(
                    socket,
                    proc,
                    ErrorCode::INTERNAL_ERROR,
                    network::server::Status::READY,
                    err);
                fs::remove_all(curdir);
                return;
            }
            auto add = network::make_additional<Server_MsgT::DATA_REPLY_EXTRACT>();
            for(auto entry:fs::directory_iterator(curdir)){
                if(entry.is_regular_file() && entry.path().extension()==".zip"){
                    add.file_sz_ = fs::file_size(curdir);
                    add.filename_ = entry.path().string();
                    add.status_ = server::Status::READY;
                }
                else{
                    send_error(
                        socket,
                        proc,
                        ErrorCode::INTERNAL_ERROR,
                        network::server::Status::READY,
                        err);
                    fs::remove_all(curdir);
                    return;
                }
                proc->send_message<Server_MsgT::DATA_REPLY_EXTRACT>(
                    stop,
                    socket,
                    err,
                    Message<Server_MsgT::DATA_REPLY_EXTRACT>(std::move(add)));
                if(err==std::error_code()){
                    std::ifstream file_stream();
                    int fno = open(curdir.c_str(),O_RDONLY|O_DIRECT);
                    if(fno==-1){
                        send_error(
                            socket,
                            proc,
                            ErrorCode::INTERNAL_ERROR,
                            server::Status::READY,
                            err);
                        fs::remove_all(curdir);
                        return;
                    }
                    auto file_add = make_additional<Server_MsgT::DATA_REPLY_FILEPART>();
                    file_add.file_hash_ = std::hash<fs::path>()(entry.path());
                    file_add.status_=server::Status::READY;
                    //@todo set user-optional buffer size by config
                    auto ptr = (char*)mmap(nullptr,add.file_sz_,PROT_READ,MAP_PRIVATE,fno,0);
                    if(ptr==nullptr){
                        send_error(
                            socket,
                            proc,
                            ErrorCode::INTERNAL_ERROR,
                            server::Status::READY,
                            err);
                        std::cout<<strerror(errno)<<std::endl;
                        errno = 0;
                        return;
                    }
                    if(int res = madvise(ptr,add.file_sz_,MADV_SEQUENTIAL);res==-1){
                        send_error(
                            socket,
                            proc,
                            ErrorCode::INTERNAL_ERROR,
                            server::Status::READY,
                            err);
                        std::cout<<strerror(errno)<<std::endl;
                        errno = 0;
                        return;
                    }
                    for(uint64_t offset=0;offset<add.file_sz_;++offset){
                        file_add.offset_=offset;
                        file_add.assign_file_segment(ptr,8912);
                        proc->send_message<Server_MsgT::DATA_REPLY_FILEPART>(
                            stop,
                            socket,
                            err,
                            std::move(file_add));
                    }
                    munmap(ptr,add.file_sz_);
                    fs::remove_all(curdir);
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
    std::error_code err;
    assert(proc->is_busy(err) && err==std::error_code());
}