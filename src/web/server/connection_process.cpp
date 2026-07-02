#include "proc/extract.h"
#include "proc/index.h"
#include "network/abstractprocess.h"
#include "web/server/connection_process.h"
#include "program/mashroom.h"
#include "sys/application.h"
#include "sys/config.h"

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
    const auto& transaction = get_reply(msg.transaction());
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

std::expected<
    Message<Server_MsgT::INDEX>,
    std::error_code> __index_process__(
    std::stop_token stop,
    ClientAppMsg appmsg) noexcept
{   std::error_code err;
    auto into = [&stop,&err](auto& in_app_msg) noexcept->
            std::expected<
            Message<Server_MsgT::INDEX>,
            std::error_code>
    {
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(in_app_msg)>>){
            return std::unexpected(std::make_error_code(std::errc::bad_message));
        }
        else{
            auto handle_msg = [&stop,&err]
                <Client_MsgT::type MSG_T>
                (const Message<MSG_T>& msg) noexcept ->
                    std::expected<
                    Message<Server_MsgT::INDEX>,
                    std::error_code>
            {
                if constexpr (MSG_T==Client_MsgT::INDEX ||
                    MSG_T==Client_MsgT::INDEX_REF)
                {
                    auto result = index_process(err,stop,msg);
                    return result;
                }
                else return std::unexpected(std::make_error_code(std::errc::bad_message));
            };
            return handle_msg(in_app_msg);
        }
    };
    return std::visit(into,appmsg);
}

std::expected<
    Message<Server_MsgT::FILE_METADATA>,
    std::error_code> extract_process(std::error_code& err,
        std::stop_token token,
        const Message<
        Client_MsgT::EXTRACT>& msg)
{
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
        msg.form());
    if(err)
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    std::string file_namebase = msg.transaction().hash();
    //main directory of current request with .zip file and cache-dir
    fs::path curdir = ::app().config().system_config().cache_files_directory()/file_namebase;
    fs::remove_all(curdir);
    if(!fs::create_directories(curdir))
        return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
    //@todo change further to std::error_code
    ErrorCode error_code;
    //@todo make setting temporary dir by config
    error_code = hExtract.set_out_path(curdir.c_str()); 
    if(error_code==ErrorCode::NONE)
        error_code = hExtract.execute();
    else{
        fs::remove_all(curdir);
        return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    }
    
    if(std::distance(fs::directory_iterator(curdir), fs::directory_iterator{})!=1) //only 1 zip file
        return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
    auto reply_msg = Message<Server_MsgT::FILE_METADATA>(
                get_reply(msg.transaction()));
    for(auto entry:fs::directory_iterator(curdir))
    {
        if(entry.is_regular_file() && entry.path().extension()==".zip"){
            
            reply_msg.file_size(entry.file_size());
            reply_msg.filename(entry.path());
            return reply_msg;
        }
        else return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    }
    return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
}

std::expected<
    Message<Server_MsgT::FILE_METADATA>,
    std::error_code> __extract_process__(
    std::stop_token stop,
    ClientAppMsg appmsg) noexcept
{   std::error_code err;
    auto into = [&stop,&err](auto& in_app_msg) noexcept->
            std::expected<
            Message<Server_MsgT::FILE_METADATA>,
            std::error_code>
    {
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(in_app_msg)>>){
            return std::unexpected(std::make_error_code(std::errc::bad_message));
        }
        else{
            auto handle_msg = [&stop,&err]
                <Client_MsgT::type MSG_T>
                (const Message<MSG_T>& msg) noexcept ->
                    std::expected<
                    Message<Server_MsgT::FILE_METADATA>,
                    std::error_code>
            {
                if constexpr (MSG_T==Client_MsgT::EXTRACT)
                {
                    auto result = extract_process(err,stop,msg);
                    return result;
                }
                else return std::unexpected(std::make_error_code(std::errc::bad_message));
            };
            return handle_msg(in_app_msg);
        }
    };
    return std::visit(into,appmsg);
}

void ServerConnectionProcess::__task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept{
    if(!version_ && msg_id!=Client_MsgT::ERROR && msg_id!=Client_MsgT::VERSION){
        __emplace_error__(err,
            "version interconnection not defined",
            server::Status::READY,
            ErrorCode::INVALID_CLIENT_REQUEST);
        return;
    }
    switch(msg_id){
        case Client_MsgT::CREDENTIALS:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::CREDENTIALS>();
            if(msg_ref.has_value()){
                const auto& msg_credentials = msg_ref->get();
                msg_credentials.login();
                msg_credentials.password();//save in network config database
                //@todo
                send_hmsg_.emplace_message(Message<Server_MsgT::CREDENTIALS>(0,Data_a::DENIED,false));
            }
            else{
                __emplace_error__(err,
                            "credentials message handling",
                            server::Status::READY,
                            ErrorCode::INTERNAL_ERROR);
            }
        }
        break;
        case Client_MsgT::SERVER_STATUS:
        {
            send_hmsg_.emplace_message(Message<Server_MsgT::SERVER_STATUS>(server::Status::READY));
        }
        break;
        case Client_MsgT::TRANSACTION:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::TRANSACTION>();
                if(msg_ref.has_value()){
                    const auto& msg_progress = msg_ref->get();
                    if(msg_progress.state()==Transaction::DECLINE){
                        file_sender_.reset();
                        waiting_.reset();
                        send_hmsg_.emplace_message(Message<Server_MsgT::TRANSACTION>(get_reply(msg_progress)));
                    }
                    else if(msg_progress.state()==Transaction::ACCEPT &&
                        file_sender_)
                    {
                        if(file_sender_->accepted())
                            file_sender_->next();
                        else
                            err = file_sender_->accept();
                    }
                    else err.clear();
                }
                else{
                    __emplace_error__(err,
                        "transaction message handling",
                        server::Status::READY,
                        ErrorCode::INTERNAL_ERROR);
                }
            break;
        }
        case Client_MsgT::PROGRESS:
        {
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::PROGRESS>();
            if(msg_ref.has_value()){
                const auto& msg_progress = msg_ref->get();
                auto& transaction = msg_progress.transaction();
                if(file_sender_ && has_task()){
                    auto reply_progress_msg = Message<Server_MsgT::PROGRESS>(get_reply(msg_progress));
                    if(file_sender_->meta().transaction().hash()==transaction.hash()){
                        float prog = file_sender_->progress();
                        if(1-prog>std::numeric_limits<float>::epsilon()){
                            reply_progress_msg.progress(file_sender_->progress());
                            reply_progress_msg.state(progress::State::SENDING);   
                        }
                        else{
                            reply_progress_msg.progress(1);
                            reply_progress_msg.state(progress::State::NOTHING);   
                        }
                        send_hmsg_.emplace_message(std::move(reply_progress_msg));
                    }
                    else if(false) //if transaction is equal to task transaction
                    {

                    }
                    else{
                         __emplace_error__(err,
                        "transaction "+msg_progress.hash()+" not found",
                        server::Status::READY,
                        ErrorCode::INVALID_CLIENT_REQUEST);
                    }
                }
                else{
                    __emplace_error__(err,
                    "transaction "+msg_progress.hash()+" not found",
                    server::Status::READY,
                    ErrorCode::INVALID_CLIENT_REQUEST);
                }
            }
            else{
                __emplace_error__(err,
                    "progress message handling",
                    server::Status::READY,
                    ErrorCode::INTERNAL_ERROR);
            }
        }
        break;
        case Client_MsgT::ERROR:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::ERROR>();
            if(msg_ref.has_value()){
                if(msg_ref && msg_ref->get().transaction().has_value()){
                    auto& err_msg = msg_ref->get();
                    auto& transaction = msg_ref->get().transaction().value();
                    if(file_sender_ && file_sender_->meta().hash() == transaction.hash()){
                        if(err_msg.error()!=ErrorCode())
                            file_sender_.reset();
                    }
                }
                else{
                    //@todo
                }
            }
            else{
                __emplace_error__(err,
                    "error message handling",
                    server::Status::READY,
                    ErrorCode::INTERNAL_ERROR);
            }
        }
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
                        send_hmsg_.emplace_message(std::move(reply));
                    }
                    else {
                        Message<Server_MsgT::VERSION> reply(
                            msg_version.version());
                        send_hmsg_.emplace_message(std::move(reply));
                    }
                }
                else{
                    __emplace_error__(err,
                        "version message handling",
                        server::Status::READY,
                        ErrorCode::INTERNAL_ERROR);
                }
            }
            else{
                __emplace_error__(err,
                    "version interconnection already defined",
                    server::Status::READY,
                    ErrorCode::INVALID_CLIENT_REQUEST);
            }
        }
        break;
        case Client_MsgT::EXTRACT:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::EXTRACT>();
            if(msg_ref.has_value());
                emplace_task<TaskMode::Thread>(err,
                    __extract_process__,
                        ClientAppMsg(msg_ref->get()));
            break;
        }
        case Client_MsgT::INDEX:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX>();
            if(msg_ref.has_value())
                emplace_task<TaskMode::Thread>(err,
                    __index_process__,
                        ClientAppMsg(msg_ref->get()));
        }
        break;
        case Client_MsgT::INDEX_REF:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX_REF>();
            if(msg_ref.has_value())
                emplace_task<TaskMode::Thread>(err,
                    __index_process__,
                        ClientAppMsg(msg_ref->get()));
        }
        break;
        default:
        __emplace_error__(
            err,
            "undefined message",
            server::Status::READY,
            ErrorCode::INVALID_CLIENT_REQUEST);
        break;
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
        msg_id.has_value()){
        __task__(err,msg_id.value());            
    }
    return;
}

void ServerConnectionProcess::on_write(std::error_code& err) noexcept{
    if(!send_hmsg_.has_message()){
        err = std::make_error_code(std::errc::no_message);
        return;
    }
    else io_context().send(err,std::move(send_hmsg_));
    return;
}

void ServerConnectionProcess::on_task_done(std::error_code& err) noexcept{
    
}
void ServerConnectionProcess::on_stop_requested(std::error_code& err) noexcept{

}