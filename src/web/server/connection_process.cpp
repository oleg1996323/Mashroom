#include "proc/extract.h"
#include "proc/index.h"
#include "network/abstractprocess.h"
#include "web/server/connection_process.h"
#include "program/mashroom.h"

using namespace network;

template <Client_MsgT::type MSG_T,Data_t TYPE,Data_f FORMAT>
void 
    find_data(
        const network::Message<MSG_T>& input,
        network::Message<network::Server_MsgT::INDEX>& output,
        const IndexParameters<TYPE,FORMAT>& index_param)
{
    static_assert((MSG_T==Client_MsgT::INDEX || MSG_T==Client_MsgT::INDEX_REF),
        "only client index message may be accepted");
    if constexpr (TYPE == Data_t::TIME_SERIES && FORMAT == Data_f::GRIB_v1){
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
            output.add_block<TYPE,FORMAT>(std::move(result));
        else return;
    }
    else static_assert(false);
};

template<Client_MsgT::type MSG_T>
::Message<Server_MsgT::INDEX> index_process(std::error_code& err,
        std::stop_token token,
        const Message<MSG_T>& msg)
{
    static_assert((MSG_T==Client_MsgT::INDEX || MSG_T==Client_MsgT::INDEX_REF),
        "only client index message may be accepted");
    const auto& transaction = msg.transaction().get_reply();
    ::Message<Server_MsgT::INDEX> rep_msg(transaction);
    auto find_data_proxy = [&](const auto& val){
        if constexpr (!std::is_same_v<std::monostate,std::decay_t<decltype(val)>>)
            find_data(msg,rep_msg,val);
        else return;
    };
    for(const auto& param : msg.parameters_){
        if(token.stop_requested()){
            ::Message<Server_MsgT::INDEX> err_rep(transaction);
            err_rep.state(Transaction::CANCEL);
            return err_rep;
        }
        std::visit(find_data_proxy,param);
    }
    return rep_msg;
}

void ServerConnectionProcess::__index_process__(
    std::stop_token stop,
    ClientAppMsg appmsg) noexcept
{   std::error_code err;
    auto dive_into = [this,&stop,&err](auto& in_app_msg)noexcept
    {
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(in_app_msg)>>){
            err = std::make_error_code(std::errc::bad_message);
            return;
        }
        else{
            auto handle_msg = [this,&stop,&err]
                <Client_MsgT::type MSG_T>
                (const Message<MSG_T>& msg) noexcept
            {
                if constexpr (MSG_T==Client_MsgT::INDEX ||
                    MSG_T==Client_MsgT::INDEX_REF)
                {
                    index_process(err,stop,msg);
                    return;
                }
                else err = std::make_error_code(std::errc::bad_message);
                return;
            };
            handle_msg(in_app_msg);
            return;
        }
    };
    std::visit(dive_into,appmsg);
}

void extract(std::error_code& err,const Message<
        Client_MsgT::EXTRACT>& msg)
{
    // Extract hExtract;
    // auto init_h = [&hExtract](auto& form){
    //     if constexpr(std::is_same_v<std::decay_t<decltype(form)>,std::monostate>){
    //         return ErrorPrint::print_error(
    //             ErrorCode::UNDEFINED_VALUE,
    //             "extract form type",
    //             AT_ERROR_ACTION::CONTINUE);
    //     }
    //     else{
    //         return hExtract.set_by_request(form);
    //     }
    // };
    // std::visit(init_h,
    //     msg.form());
    // if(err!=std::error_code()){
    //     send_error(
    //         socket,
    //         proc,
    //         ErrorCode::INVALID_ARGUMENT,
    //         network::server::Status::READY,
    //         err);
    //     return;
    // }
    // std::string file_namebase = std::to_string(
    //     utc_tp::clock::now().time_since_epoch().count())+"_"+
    //     socket.ip_to_text()+":"+socket.port_to_text();
    // //main directory of current request with .zip file and cache-dir
    // auto curdir = fs::temp_directory_path()/file_namebase;
    // fs::remove_all(curdir);
    // if(!fs::create_directories(curdir)){
    //     send_error(
    //             socket,
    //             proc,
    //             ErrorCode::INTERNAL_ERROR,
    //             network::server::Status::READY,
    //             err);
    //     return;
    // }
    // //@todo change further to std::error_code
    // ErrorCode error_code;
    // //@todo make setting temporary dir by config
    // error_code = hExtract.add_in_path(curdir.c_str()); 
    // if(error_code==ErrorCode::NONE)
    //     error_code = hExtract.execute();
    // else{
    //     send_error(
    //         socket,
    //         proc,
    //         ErrorCode::INTERNAL_ERROR,
    //         network::server::Status::READY,
    //         err);
    //     fs::remove_all(curdir);
    //     return;
    // }
    // auto add = network::make_additional<Server_MsgT::DATA_REPLY_EXTRACT>();
    // for(auto entry:fs::directory_iterator(curdir))
    // {
    //     if(entry.is_regular_file() && entry.path().extension()==".zip"){
    //         add.file_sz_ = fs::file_size(curdir);
    //         add.filename_ = entry.path().string();
    //         add.status_ = server::Status::READY;
    //     }
    //     else{
    //         send_error(
    //             socket,
    //             proc,
    //             ErrorCode::INTERNAL_ERROR,
    //             network::server::Status::READY,
    //             err);
    //         fs::remove_all(curdir);
    //         return;
    //     }
    //     proc->send_message<Server_MsgT::DATA_REPLY_EXTRACT>(
    //         stop,
    //         socket,
    //         err,
    //         Message<Server_MsgT::DATA_REPLY_EXTRACT>(std::move(add)));
    //     if(err==std::error_code()){
    //         std::ifstream file_stream();
    //         int fno = open(curdir.c_str(),O_RDONLY|O_DIRECT);
    //         if(fno==-1){
    //             send_error(
    //                 socket,
    //                 proc,
    //                 ErrorCode::INTERNAL_ERROR,
    //                 server::Status::READY,
    //                 err);
    //             fs::remove_all(curdir);
    //             return;
    //         }
    //         auto file_add = make_additional<Server_MsgT::DATA_REPLY_FILEPART>();
    //         file_add.file_hash_ = std::hash<fs::path>()(entry.path());
    //         file_add.status_=server::Status::READY;
    //         //@todo set user-optional buffer size by config
    //         auto ptr = (char*)mmap(nullptr,add.file_sz_,PROT_READ,MAP_PRIVATE,fno,0);
    //         if(ptr==nullptr){
    //             send_error(
    //                 socket,
    //                 proc,
    //                 ErrorCode::INTERNAL_ERROR,
    //                 server::Status::READY,
    //                 err);
    //             std::cout<<strerror(errno)<<std::endl;
    //             errno = 0;
    //             return;
    //         }
    //         if(int res = madvise(ptr,add.file_sz_,MADV_SEQUENTIAL);res==-1){
    //             send_error(
    //                 socket,
    //                 proc,
    //                 ErrorCode::INTERNAL_ERROR,
    //                 server::Status::READY,
    //                 err);
    //             std::cout<<strerror(errno)<<std::endl;
    //             errno = 0;
    //             return;
    //         }
    //         for(uint64_t offset=0;offset<add.file_sz_;++offset){
    //             file_add.offset_=offset;
    //             file_add.assign_file_segment(ptr,8912);
    //             proc->send_message<Server_MsgT::DATA_REPLY_FILEPART>(
    //                 stop,
    //                 socket,
    //                 err,
    //                 std::move(file_add));
    //         }
    //         munmap(ptr,add.file_sz_);
    //         fs::remove_all(curdir);
    //     }
    // }
}

void ServerConnectionProcess::__task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept{
    switch(msg_id){
        case Client_MsgT::CREDENTIALS:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::CREDENTIALS>();
            if(msg_ref.has_value()){
                const auto& msg_credentials = msg_ref->get();
                msg_credentials.login();
                msg_credentials.password();//save in network config database
                //@todo
                io_context().serialize(Message<Server_MsgT::CREDENTIALS>(0,Data_a::DENIED,false));
            }
            else{
                __enqueue_error__(err,
                            "credentials message handling",
                            server::Status::READY,
                            ErrorCode::INTERNAL_ERROR);
            }
        }
        break;
        case Client_MsgT::SERVER_STATUS:
        {
            io_context().serialize(Message<Server_MsgT::SERVER_STATUS>(server::Status::READY));
        }
        break;
        case Client_MsgT::TRANSACTION:
        break;
        case Client_MsgT::PROGRESS:
        {
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::PROGRESS>();
            if(msg_ref.has_value()){
                const auto& msg_progress = msg_ref->get();
                io_context().serialize(Message<Server_MsgT::PROGRESS>(msg_progress.get_reply()));
            }
            else{
                __enqueue_error__(err,
                    "progress message handling",
                    server::Status::READY,
                    ErrorCode::INTERNAL_ERROR);
            }
        }
        break;
        case Client_MsgT::ERROR:
            //@todo
        break;
        case Client_MsgT::VERSION:
        {
            if(!version_.has_value()){
                auto msg_ref = recv_hmsg_.get_message<Client_MsgT::VERSION>();
                if(msg_ref.has_value()){
                    const auto& msg_version = msg_ref->get();
                    if(msg_version.version()>=
                        app().config().system_config().version())
                    {
                        Message<Server_MsgT::VERSION> reply(
                            app().config().system_config().version());
                        io_context().serialize(reply);
                    }
                    else {
                        Message<Server_MsgT::VERSION> reply(
                            msg_version.version());
                        io_context().serialize(reply);
                    }
                }
                else{
                    __enqueue_error__(err,
                        "version message handling",
                        server::Status::READY,
                        ErrorCode::INTERNAL_ERROR);
                }
            }
            else{
                __enqueue_error__(err,
                    "version interconnection already defined",
                    server::Status::READY,
                    ErrorCode::INVALID_CLIENT_REQUEST);
            }
        }
        break;
        case Client_MsgT::EXTRACT:
            //heavy task
            break;
        case Client_MsgT::INDEX:

        case Client_MsgT::INDEX_REF:
        auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX_REF>()->get();
        emplace_binded_task<TaskMode::Thread>(err,
            &ServerConnectionProcess::__index_process__,
                this,msg_ref);
    }
}

void ServerConnectionProcess::on_read(std::error_code& err) noexcept{
    using namespace serialization;
    err.clear();
    io_context().receive(err,recv_hmsg_);
    if(err) {
        handle_receive_error(err);
        return;
    }
    if(auto msg_id = recv_hmsg_.message_type();
        msg_id.has_value())

        
    return;
}

void ServerConnectionProcess::on_write(std::error_code& err) noexcept{
    if(!send_hmsg_.has_message()){
        err = std::make_error_code(std::errc::no_message);
        return;
    }
    return;
}

void ServerConnectionProcess::on_task_done(std::error_code& err) noexcept{
    
}
void ServerConnectionProcess::on_stop_requested(std::error_code& err) noexcept{

}