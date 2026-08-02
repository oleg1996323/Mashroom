#include "proc/extract.h"
#include "proc/index.h"
#include "OsterLib/network/abstractprocess.h"
#include "web/server/connection_process.h"
#include "program/mashroom.h"
#include "sys/application.h"
#include "sys/config.h"
#include "OsterLib/contexted_error.h"
#include "common/MessagePositionSizeInfo.h"
#include <optional>

using namespace network;

constexpr size_t threaded_min_number_sizes = 10000000;
constexpr size_t threaded_min_number_files = 10000;
constexpr size_t max_file_part = 8192*100;

using FilePathsPositionsSizes = std::vector<std::pair<Location<true>,
            std::vector<MessagePositionSizeInfo>>>;

template<network::Server_MsgT::type MSG,typename Additional>
struct TaskResultMessage
{
    constexpr static network::Server_MsgT::type msg_id = MSG;
    using msg_t = network::Message<MSG>;
    using additional_t = Additional;
    network::Message<MSG> msg_;
    Additional add_;
};

using task_index_result_t = TaskResultMessage<
        network::Server_MsgT::INDEX,
        std::optional<FilePathsPositionsSizes>>;

using task_rawdata_parts_result_t = TaskResultMessage<
        network::Server_MsgT::RAWDATA_PARTS_METADATA,
        ServerConnectionProcess::SendingFileState>;

using task_file_metadata_result_t = TaskResultMessage<
        network::Server_MsgT::FILE_METADATA,
        ServerConnectionProcess::SendingFileState>;

using task_result_t = 
        std::variant<std::monostate,
        task_index_result_t,
        task_rawdata_parts_result_t,
        task_file_metadata_result_t>;

template <Client_MsgT::type MSG_T,Data_t TYPE,Data_f FORMAT>
void 
    find_data(
        FilePathsPositionsSizes& pos,
        const network::Message<MSG_T>& input,
        network::Message<network::Server_MsgT::INDEX>& output,
        const IndexParameters<TYPE,FORMAT>& index_param)
{
    static_assert((MSG_T==Client_MsgT::INDEX || MSG_T==Client_MsgT::INDEX_REF),
        "only client index message may be accepted");
    if constexpr (TYPE == Data_t::TIME_SERIES && FORMAT == Data_f::GRIB_v1){
        auto result = Mashroom::instance().data().
            data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().find_all(
            pos,
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

std::expected<
    task_result_t,
    osterlib::ContextedError> __index_process__(
    std::stop_token stop,
    ClientAppMsg appmsg,
    bool send_file) noexcept
{   
    auto into = [&stop,send_file](auto& in_app_msg) noexcept->
            std::expected<
            task_result_t,
            osterlib::ContextedError>
    {
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(in_app_msg)>>){
            return std::unexpected(
                    osterlib::ContextedError(
                        mashroom::errc::invalid_argument,"monostate passed to visitor"));
        }
        else{
            auto handle_msg = [&stop,send_file]
                <Client_MsgT::type MSG_T>
                (const Message<MSG_T>& msg) noexcept ->
                    std::expected<
                    task_result_t,
                    osterlib::ContextedError>
            {
                if constexpr (MSG_T==Client_MsgT::INDEX ||
                    MSG_T==Client_MsgT::INDEX_REF)
                {
                    FilePathsPositionsSizes pos;
                    MessageHandler<Side::SERVER> result;
                    const auto& transaction = get_reply(msg.transaction());
                    ::Message<Server_MsgT::INDEX> rep_msg(transaction);
                    auto find_data_proxy = [&](const auto& val) noexcept{
                        if constexpr (!std::is_same_v<std::monostate,std::decay_t<decltype(val)>>){
                            find_data(pos,msg,rep_msg,val);
                        }
                        else return;
                    };
                    for(const auto& param : msg.parameters_){
                        if(stop.stop_requested()){
                            task_index_result_t result{
                                .msg_=::Message<Server_MsgT::INDEX>(transaction),
                                .add_=std::nullopt};
                            result.msg_.state(Transaction::CANCEL);
                            return result;
                        }
                        std::visit(find_data_proxy,param);
                    }
                    if(send_file)
                        return task_index_result_t{.msg_=std::move(rep_msg),.add_=std::move(pos)};
                    else return task_index_result_t{.msg_=std::move(rep_msg),.add_=std::nullopt};
                }
                else{
                    osterlib::ContextedError ctx_err(
                        mashroom::errc::invalid_argument,
                        "invalid message tag");
                    ctx_err.with_field("procedure","index")
                    .with_field("tag",MSG_T);
                    return std::unexpected(std::move(ctx_err));
                }
            };
            return handle_msg(in_app_msg);
        }
    };
    return std::visit(into,appmsg);
}

namespace details{

std::expected<
    task_file_metadata_result_t,
    osterlib::ContextedError> extract_process_internal(
        std::stop_token token,
        const Message<
        Client_MsgT::EXTRACT>& msg)
{
    Extract hExtract;
    auto init_h = [&hExtract](auto& form){
        if constexpr(std::is_same_v<std::decay_t<decltype(form)>,std::monostate>)
            return osterlib::ContextedError(mashroom::errc::invalid_argument,"monostate passed to visitor")
                    .with_field("procedure","extract")
                    .with_field("at","resolve extract form");
        else return hExtract.set_by_request(form);
    };
    osterlib::ContextedError ctx_err = std::visit(init_h,msg.form());
    if(ctx_err)
        return std::unexpected(std::move(ctx_err));
    std::string file_namebase = msg.transaction().hash();
    //main directory of current request with .zip file and cache-dir
    fs::path curdir = ::app().config().system_config().cache_files_directory()/file_namebase;
    fs::remove_all(curdir);
    if(!fs::create_directories(curdir)){
        ctx_err.error(mashroom::errc::create_directory_denied)
        .with_field("procedure","extract")
        .with_field("dir",curdir.c_str());
        return std::unexpected(std::move(ctx_err));
    }
    //@todo make setting temporary dir by config 
    if(auto code = hExtract.set_out_path(curdir.c_str());code){
        fs::remove_all(curdir);
        ctx_err.error(code)
        .with_field("procedure","extract")
        .with_field("dir",curdir.c_str());
        return std::unexpected(std::move(ctx_err));
    }
    else ctx_err = hExtract.execute();
    if(ctx_err)
        return std::unexpected(std::move(ctx_err));
    
    if(std::distance(fs::directory_iterator(curdir), fs::directory_iterator{})!=1) //only 1 zip file
    {
        ctx_err.error(mashroom::errc::internal_error,"more than package file in destination directory")
        .with_field("procedure","extract");
        return std::unexpected(std::move(ctx_err));
    }
    task_file_metadata_result_t result{.msg_ = Message<Server_MsgT::FILE_METADATA>(
                    get_reply(msg.transaction())),
                .add_=ServerConnectionProcess::SendingFileState(
                    get_reply(msg.transaction()),max_file_part)};
    for(auto entry:fs::directory_iterator(curdir))
    {
        if(entry.is_regular_file())
        {
            if(entry.path().extension()==".zip"){    
                result.msg_.file_size(entry.file_size());
                result.msg_.filename(entry.path());
                result.add_.append_filepart(
                        entry.path().string(),
                        0,
                        fs::file_size(entry.path()));
                return result;
            }
            else{
                ctx_err.error(mashroom::errc::unknown_file_format,"not zip format");
                ctx_err.with_field("procedure","extract")
                .with_field("file",entry.path().c_str());
                return std::unexpected(std::move(ctx_err));
            }
        }
        else{
            ctx_err.error(mashroom::errc::not_file);
            ctx_err.with_field("procedure","extract")
                .with_field("path",entry.path().c_str());
            return std::unexpected(std::move(ctx_err));
        }
    }
    ctx_err.error(mashroom::errc::not_file_or_directory,"empty directory");
    ctx_err.with_field("procedure","extract")
        .with_field("dir",curdir.c_str());
    return std::unexpected(std::move(ctx_err));
}
}

std::expected<
    task_result_t,
    osterlib::ContextedError> __extract_process__(
    std::stop_token stop,
    ClientAppMsg appmsg) noexcept
{   std::error_code err;
    auto into = [&stop,&err](auto& in_app_msg) noexcept->
            std::expected<
            task_result_t,
            osterlib::ContextedError>
    {
        osterlib::ContextedError ctx_err;
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(in_app_msg)>>){
            ctx_err.error(mashroom::errc::not_file_or_directory,
                    "monostate passed to visitor");
            ctx_err.with_field("procedure","extract")
                .with_field("at","resolve message handler category");
            return std::unexpected(std::move(ctx_err));
        }
        else{
            auto handle_msg = [&stop,&ctx_err]
                <Client_MsgT::type MSG_T>
                (const Message<MSG_T>& msg) noexcept ->
                    std::expected<
                    task_result_t,
                    osterlib::ContextedError>
            {
                if constexpr (MSG_T==Client_MsgT::EXTRACT)
                {
                    if(std::expected<task_file_metadata_result_t,
                        osterlib::ContextedError> result = 
                            details::extract_process_internal(stop,msg);result)
                        return task_result_t(std::move(result.value()));
                    else return std::unexpected(std::move(result.error()));
                }
                else{
                    ctx_err.error(
                        mashroom::errc::invalid_argument,
                        "invalid message tag");
                    ctx_err.with_field("procedure","extract")
                    .with_field("tag",MSG_T);
                    return std::unexpected(std::move(ctx_err));
                }
            };
            return handle_msg(in_app_msg);
        }
    };
    return std::visit(into,appmsg);
}

template<Server_MsgT::type MSG>
MessageHandler<Side::SERVER> next_message_spec(const Message<MSG>& msg) noexcept{
    return {};
}
MessageHandler<Side::SERVER> next_message(MessageHandler<Side::SERVER>&& sent) noexcept{
    auto visitor = [](auto& msg_cat) noexcept
        ->MessageHandler<Side::SERVER>
    {
        using category = std::decay_t<decltype(msg_cat)>;
        if constexpr(std::is_same_v<std::monostate,category>)
            return {};
        else{
            auto visitor_cat = [](const auto& msg) noexcept
                ->MessageHandler<Side::SERVER>
            {
                using msg_t = std::decay_t<decltype(msg)>;
                if constexpr(std::is_same_v<std::monostate,msg_t>)
                    return {};
                else{
                    return []<Server_MsgT::type TYPE>
                        (const Message<TYPE>& msg_spec) noexcept
                        ->MessageHandler<Side::SERVER>
                    {
                        return next_message_spec<TYPE>(msg_spec);
                    }(msg);
                }
            };
            return std::visit(visitor_cat,msg_cat);
        }
    };
    return std::visit(visitor,sent.data());
}

void task_done_callback(ConnectionHandle hconn) noexcept
{
    std::error_code err;
    hconn.execute_command(
        std::make_shared<Command<CommandType::TaskDone>>(hconn),
        err);
}

void ServerConnectionProcess::__task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept{
    if((!version_.has_value() && msg_id!=Client_MsgT::VERSION) && msg_id!=Client_MsgT::ERROR){
        __emplace_error__(err,
            "version interconnection not defined",
            server::Status::READY,
            mashroom::network::errc::invalid_client_request);
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
                            mashroom::network::errc::internal_error);
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
                        file_sender_.erase(msg_progress.hash());
                        send_hmsg_.emplace_message(Message<Server_MsgT::TRANSACTION>(get_reply(msg_progress)));
                    }
                    else if(msg_progress.state()==Transaction::ACCEPT &&
                        file_sender_.contains(msg_progress.hash()))
                    {
                        if(!file_sender_.at(msg_progress.hash()).accepted())
                            err = file_sender_.at(msg_progress.hash()).accept();                            
                    }
                    else err.clear();
                }
                else{
                    __emplace_error__(err,
                        "transaction message handling",
                        server::Status::READY,
                        mashroom::network::errc::internal_error);
                }
            break;
        }
        case Client_MsgT::PROGRESS:
        {
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::PROGRESS>();
            if(msg_ref.has_value()){
                const auto& msg_progress = msg_ref->get();
                auto& transaction = msg_progress.transaction();
                if(file_sender_.contains(msg_progress.hash()) && has_task()){
                    auto reply_progress_msg = Message<Server_MsgT::PROGRESS>(get_reply(msg_progress));
                    if(file_sender_.at(msg_progress.hash()).transaction().hash()==transaction.hash()){
                        float prog = file_sender_.at(msg_progress.hash()).progress();
                        if(1-prog>std::numeric_limits<float>::epsilon()){
                            reply_progress_msg.progress(file_sender_.
                                at(msg_progress.hash()).progress());
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
                        mashroom::network::errc::invalid_client_request);
                    }
                }
                else{
                    __emplace_error__(err,
                    "transaction "+msg_progress.hash()+" not found",
                    server::Status::READY,
                    mashroom::network::errc::invalid_client_request);
                }
            }
            else{
                __emplace_error__(err,
                    "progress message handling",
                    server::Status::READY,
                    mashroom::network::errc::internal_error);
            }
        }
        break;
        case Client_MsgT::ERROR:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::ERROR>();
            if(msg_ref.has_value()){
                if(msg_ref && msg_ref->get().transaction().has_value()){
                    auto& err_msg = msg_ref->get();
                    auto& transaction = msg_ref->get().transaction().value();
                    if(file_sender_.contains(transaction.hash())){
                        if(err_msg.error()!=mashroom::network::errc())
                            file_sender_.erase(transaction.hash());
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
                    mashroom::network::errc::internal_error);
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
                        version_= app().config().system_config().version();
                        Message<Server_MsgT::VERSION> reply(
                            app().config().system_config().version());
                        send_hmsg_.emplace_message(std::move(reply));
                    }
                    else {
                        version_= msg_version.version();
                        Message<Server_MsgT::VERSION> reply(
                            msg_version.version());
                        send_hmsg_.emplace_message(std::move(reply));
                    }
                }
                else{
                    __emplace_error__(err,
                        "version message handling",
                        server::Status::READY,
                        mashroom::network::errc::internal_error);
                }
            }
            else{
                __emplace_error__(err,
                    "version interconnection already defined",
                    server::Status::READY,
                    mashroom::network::errc::invalid_client_request);
            }
        }
        break;
        case Client_MsgT::EXTRACT:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::EXTRACT>();
            if(msg_ref.has_value())
                emplace_task<TaskMode::Thread>(
                    [hconn=connection_handle()](){
                        task_done_callback(hconn);
                    },
                    err,
                    __extract_process__,
                        ClientAppMsg(msg_ref->get()));
            
        }
        break;
        case Client_MsgT::INDEX:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX>();
            if(msg_ref.has_value())
                emplace_task<TaskMode::Thread>(
                    [hconn=connection_handle()](){
                        task_done_callback(hconn);
                    },
                    err,
                    __index_process__,
                        ClientAppMsg(msg_ref->get()),true);
        }
        break;
        case Client_MsgT::INDEX_REF:{
            auto msg_ref = recv_hmsg_.get_message<Client_MsgT::INDEX_REF>();
            if(msg_ref.has_value())
                emplace_task<TaskMode::Thread>([hconn=connection_handle()]() mutable
                    {
                        std::error_code err;
                        hconn.execute_command(
                            std::make_shared<Command<CommandType::TaskDone>>(hconn),
                            err);
                    },
                    err,
                    __index_process__,
                        ClientAppMsg(msg_ref->get()),false);
        }
        break;
        default:
        __emplace_error__(
            err,
            "undefined message",
            server::Status::READY,
            mashroom::network::errc::invalid_client_request);
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
        on_write(err);        
    }
    return;
}

void ServerConnectionProcess::on_write(std::error_code& err) noexcept{
    io_context().send_rest(err);
    if(!handle_sending_error(err))
        return;
    if(send_hmsg_.has_message()){
        io_context().send(err,[](const std::vector<char>&){},send_hmsg_);
        send_hmsg_.clear();
    }
    if(!handle_sending_error(err))
        return;
    for(auto& [hash,fsender]:file_sender_){
        if(fsender.accepted() && fsender.has_to_send())
            fsender.next();
    }
    return;
}

std::expected<task_result_t,
    osterlib::ContextedError> prepare_index_metadata(
        std::stop_token stop,
        Message<Server_MsgT::TRANSACTION> transaction,
        FilePathsPositionsSizes fpps) noexcept{
    task_rawdata_parts_result_t result{.msg_=
        Message<Server_MsgT::RAWDATA_PARTS_METADATA>(transaction),
        .add_=std::move(ServerConnectionProcess::SendingFileState(
            std::move(transaction),max_file_part))};
    boost::uuids::detail::sha1 s;
    size_t sz = 0;
    for(auto& [fn,pos_size]:fpps){
        if(fn.is_file() &&
            fs::exists(fn.path()) &&
            fs::is_regular_file(fn.path()))
        {
            std::ifstream file(fn.path().data());
            for(auto& [pos,size]:pos_size){
                sz+=size;
                file.seekg(pos,std::istream::beg);
                if(size>max_file_part){
                    std::vector<char> buf(max_file_part);
                    for(size_t i=0;i<size;i+=max_file_part){
                        size_t to_read = 0;
                        if(size-i>max_file_part)
                            to_read = max_file_part;
                        else
                            to_read = size-i;
                        file.read(buf.data(),to_read);
                        if(file.fail())
                        {
                            ;
                            return std::unexpected(
                                osterlib::ContextedError(mashroom::errc::file_reading_error)
                                    .with_field("at","prepare index metadata")
                                    .with_field("file",fn.path()));
                        }
                        else s.process_bytes(buf.data(),to_read);
                    }
                }
                else{
                    std::vector<char> buf(size);
                    file.read(buf.data(),size);
                    if(file.fail())
                    {
                        return std::unexpected(
                            osterlib::ContextedError(mashroom::errc::file_reading_error)
                        .with_field("at","prepare index metadata")
                        .with_field("file",fn.path()));
                    }
                    else s.process_bytes(buf.data(),size);
                }
                result.add_.append_filepart(fn.path().data(),pos,size);
            }
        }
    }
    result.msg_.data_size(sz);
    crypto::SHA1 digest;
    s.get_digest(digest);
    result.msg_.digest(digest);
    return task_result_t(std::move(result));
}

void ServerConnectionProcess::on_task_done(std::error_code& err) noexcept{
    std::cout<<"Task done"<<std::endl;
    std::unique_ptr<network::AbstractTaskHandler> task(task_.release());
    if(auto task_result_ = const_cast<std::expected<task_result_t, osterlib::ContextedError> *>(
            task->get_as_ptr<std::expected<task_result_t, osterlib::ContextedError>>(err));
        task_result_!=nullptr)
    {
        if(send_hmsg_.has_message()){
            connection_handle().execute_command(
                std::make_shared<Command<CommandType::TaskDone>>(connection_handle()),err);
            std::cout<<"Busy. TaskDone trying again"<<std::endl;
        }
        else {
            if(task_result_->has_value()){
                auto visit_result = [this](auto&& result){
                    std::error_code err_local;
                    if constexpr (std::is_same_v<task_index_result_t,std::decay_t<decltype(result)>>){
                        auto transaction = result.msg_.transaction();
                        send_hmsg_.emplace_message(std::move(result.msg_));
                        std::error_code err_local;
                        if(result.add_.has_value() && 
                            result.add_->size()>0)
                        {
                            emplace_task<TaskMode::Thread>(
                                [hconn=connection_handle()](){
                                    task_done_callback(hconn);
                                },
                                err_local,
                                prepare_index_metadata,
                                std::move(transaction),
                                std::move(result.add_.value()));
                            return;
                        }
                        else return;
                    }
                    else if constexpr(std::is_same_v<task_rawdata_parts_result_t,
                            std::decay_t<decltype(result)>> ||
                            std::is_same_v<task_file_metadata_result_t,
                            std::decay_t<decltype(result)>>)
                    {
                        auto t = result.msg_.transaction();
                        send_hmsg_.emplace_message(std::move(result.msg_));
                        file_sender_.insert(std::make_pair(t.hash(),
                            std::move(result.add_)));                 
                    }
                    else return;
                };
                task_result_t value(std::move(const_cast<
                        std::decay_t<decltype(task_result_->value())>&&>(task_result_->value())));
                
                std::visit(visit_result,std::move(value));
            }
            else {
                __emplace_error__(
                err,
                "operation error",
                server::Status::READY,
                mashroom::network::errc::internal_error);
                send_hmsg_.clear();
            }
        }
        on_write(err);
    }
    else std::cout<<"Bad task result"<<std::endl;
}
void ServerConnectionProcess::on_stop_requested(std::error_code& err) noexcept{

}

static_assert(std::is_constructible_v<std::expected<task_result_t, osterlib::ContextedError>,
        const std::expected<task_result_t, osterlib::ContextedError>&>);

static_assert(std::is_default_constructible_v<task_result_t>);
static_assert(std::is_default_constructible_v<std::expected<task_result_t, osterlib::ContextedError>>);