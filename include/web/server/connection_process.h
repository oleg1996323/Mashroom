#pragma once
#include "web/common/msgdef.h"
#include <string_view>
#include <memory>
#include <fstream>
#include <list>
#include <vector>

#include "OsterLib/network/abstractprocess.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "OsterLib/network/commonsocket.h"
#include "OsterLib/network/multiplexor/eventhandle.h"
#include "common/MessagePositionSizeInfo.h"

namespace network{
    // template<network::Server_MsgT::type MSG,typename Additional>
    // struct TaskResultMessage
    // {
    //     constexpr static network::Server_MsgT::type msg_id = MSG;
    //     using msg_t = Message<MSG>;
    //     using additional_t = Additional;
    //     Message<MSG> msg_;
    //     Additional add_;
    // };

    class ServerConnectionProcess:public AbstractConnectionProcess
    {
        public:
        class SendingFileState{
            ::std::deque<std::pair<::std::string_view,
                ::std::deque<MessagePositionSizeInfo>>> files_;
            Message<Server_MsgT::TRANSACTION> transaction_;
            ::std::ifstream stream_;
            ::boost::uuids::detail::sha1 summary_;
            size_t chunk_sz_=0;
            size_t has_read_=0;
            size_t size_ = 0;
            size_t remain_ = 0;
            SendingFileState() = default;
            // friend class TaskResultMessage<
            // network::Server_MsgT::RAWDATA_PARTS_METADATA,
            // ServerConnectionProcess::SendingFileState>;
            // friend class TaskResultMessage<
            // network::Server_MsgT::FILE_METADATA,
            // ServerConnectionProcess::SendingFileState>;
            public:
            SendingFileState(
                Message<Server_MsgT::TRANSACTION> transaction,
                size_t chunk):
                transaction_(std::move(transaction)),
                chunk_sz_(chunk)
            {}
            SendingFileState(
                ::std::error_code& err,
                ::std::string_view filename,
                Message<Server_MsgT::TRANSACTION> transaction,
                size_t start,
                size_t size,
                size_t chunk
                ) noexcept:
                files_({{std::move(filename),{MessagePositionSizeInfo{.begin_=start,.size_=size}}}}),
                transaction_(std::move(transaction)),
                chunk_sz_(chunk),
                size_(size),
                remain_(size)
            {
                transaction.state(Transaction::NEW);
                if(fs::exists(filename) || 
                    fs::is_regular_file(filename)){
                    stream_ = ::std::ifstream(filename.data());
                    stream_.seekg(start,::std::ios::beg);
                    if(stream_.fail())
                        err = ::std::make_error_code(::std::errc::io_error);
                }
                else return;
            }
            SendingFileState(const SendingFileState& other) noexcept:
                files_(other.files_),
                transaction_(other.transaction_),
                summary_(other.summary_),
                chunk_sz_(other.chunk_sz_),
                has_read_(other.has_read_),
                size_(other.size_),
                remain_(other.remain_)
                {
                    if(other.stream_.is_open() && 
                        !files_.empty()){
                        stream_.open(files_.front().first.data());
                        if(stream_.is_open()){
                            stream_.seekg(files_.front().second.front().begin_+has_read_);
                        }
                    }
                }
            SendingFileState& operator=(const SendingFileState& other) noexcept
            {
                if(this!=&other){
                    files_ = other.files_;
                    transaction_ = other.transaction_;                  
                    summary_ = other.summary_;
                    chunk_sz_ = other.chunk_sz_;
                    has_read_ = other.has_read_;
                    size_ = other.size_;
                    remain_ = other.remain_;
                    if(other.stream_.is_open() && 
                        !files_.empty()){
                        stream_.open(files_.front().first.data());
                        if(stream_.is_open()){
                            stream_.seekg(files_.front().second.front().begin_+has_read_);
                        }
                    }
                }
                return *this;
            }
            SendingFileState(SendingFileState&& other) noexcept:
                files_(std::move(other.files_)),
                transaction_(std::move(other.transaction_)),
                stream_(std::move(other.stream_)),
                summary_(std::move(other.summary_)),
                chunk_sz_(other.chunk_sz_),
                has_read_(other.has_read_),
                size_(other.size_),
                remain_(other.remain_){}
            SendingFileState& operator=(SendingFileState&& other) noexcept
            {
                if(this!=&other){
                    files_ = std::move(other.files_);
                    transaction_ = std::move(other.transaction_);
                    stream_ = std::move(other.stream_);
                    summary_ = std::move(other.summary_);
                    chunk_sz_ = other.chunk_sz_;
                    has_read_ = other.has_read_;
                    size_ = other.size_;
                    remain_ = other.remain_;
                }
                return *this;
            }
            //push back filename and size+start position
            ::std::error_code append_filepart(
                ::std::string_view filename,
                size_t start,
                size_t size) noexcept
            {
                if(!filename.empty() &&
                    fs::exists(filename) && 
                    fs::is_regular_file(filename))
                {
                    size_+=size;
                    remain_+=size;
                    files_.push_back(std::make_pair(
                            filename,
                            ::std::deque<MessagePositionSizeInfo>{
                                MessagePositionSizeInfo{.begin_=start,.size_=size}
                            }));
                    return ::std::error_code();
                }
                else return ::std::make_error_code(::std::errc::invalid_argument);
            }
            //add size and start position if filename exists
            bool add_filepart_to_file(
                ::std::string_view filename,
                size_t start,
                size_t size) noexcept
            {
                if(auto found = std::find_if(
                    files_.rbegin(),
                    files_.rend(),
                    [&filename](
                        const std::pair<std::string_view,
                        std::deque<MessagePositionSizeInfo>>& val)
                    {
                        return filename==val.first;
                    });
                    found!=files_.rend())
                {
                    size_+=size;
                    remain_+=size;
                    found->second.push_back(MessagePositionSizeInfo{.begin_=start,.size_=size});
                    return true;
                }
                else return false;
                
            }
            const Message<Server_MsgT::TRANSACTION>& transaction() const noexcept{
                return transaction_;
            }
            bool valid() const noexcept{
                return stream_.is_open();
            }
            bool has_to_send() const noexcept{
                return !files_.empty();
            }
            size_t size() const noexcept{
                return size_;
            }
            size_t remain() const noexcept{
                return remain_;
            }
            size_t chunk() const noexcept{
                return chunk_sz_;
            }
            size_t has_read() const noexcept{
                return has_read_;
            }
            bool accepted() const noexcept{
                return transaction_.state()==Transaction::ACCEPT;
            }
            bool canceled() const noexcept{
                return transaction_.state()==Transaction::CANCEL;
            }
            bool is_waiting_accept() const noexcept{
                return transaction_.state()==Transaction::NEW;
            }
            std::error_code accept() noexcept{
                if(canceled())
                    return std::make_error_code(std::errc::operation_not_permitted);
                else if(accepted())
                    return std::make_error_code(std::errc::operation_not_permitted);
                else return std::error_code();
            }
            void cancel() noexcept{
                transaction_.state(Transaction::CANCEL);
            }
            std::expected<Message<Server_MsgT::FILE_DATA>,std::error_code> next() noexcept{
                if(accepted() && has_to_send()){
                    Message<Server_MsgT::FILE_DATA> result(transaction_);
                    if((size()-has_read_)<=chunk_sz_){
                        std::vector<char> buffer(chunk_sz_);
                        stream_.read(buffer.data(),chunk_sz_);
                        result.data(std::move(buffer));
                        return result;
                    }
                    else{
                        size_t to_read = size()-has_read_;
                        std::vector<char> buffer(to_read);
                        stream_.read(buffer.data(),to_read);
                        if(stream_.fail())
                            return std::unexpected(
                                std::make_error_code(std::errc::operation_canceled));
                        result.data(std::move(buffer));
                        return result;
                    }
                }
                else return std::unexpected(
                        std::make_error_code(
                            std::errc::operation_not_permitted));
            }
            float progress() const noexcept{
                return float(has_read())/size();
            }
        };
        private:
        MessageHandler<Side::CLIENT> recv_hmsg_;
        MessageHandler<Side::SERVER> send_hmsg_;
        std::optional<size_t> version_; //@todo
        std::optional<Data_a> access_;
        std::unordered_map<std::string,SendingFileState> file_sender_;
        void __task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept;
        void __emplace_error__(std::error_code& err,
                std::string description,
                server::Status status,
                mashroom::errc code) noexcept
        {
            send_hmsg_.emplace_message(
                Message<Server_MsgT::ERROR>(
                    code,
                    ErrorPrint::message(
                        code,
                        std::move(description)),
                        status
                ));
            io_context().send(err,[](const std::vector<char>&){},send_hmsg_);
            send_hmsg_.clear();
        }
        void __emplace_error__(std::error_code& err,
                std::string description,
                server::Status status,
                Message<Server_MsgT::TRANSACTION> transaction,
                mashroom::errc code) noexcept
        {
            send_hmsg_.emplace_message(Message<Server_MsgT::ERROR>(
                    mashroom::errc::INTERNAL_ERROR,
                    ErrorPrint::message(
                        code,
                        std::move(description)),
                        std::move(transaction),
                        status
                    ));
            io_context().send(err,[](const std::vector<char>&){},send_hmsg_);
            send_hmsg_.clear();
        }
        public:
        virtual void on_read(std::error_code& err) noexcept override;
        virtual void on_write(std::error_code& err) noexcept override;
        virtual void on_task_done(std::error_code& err) noexcept override;
        virtual void on_stop_requested(std::error_code& err) noexcept override;
        void at_fatal_error(std::error_code& err) noexcept override{
            err.clear();
            io_context().clear_buffers();
            __emplace_error__(err,
                "something gone wrong",
                server::Status::READY,
                mashroom::errc::INTERNAL_ERROR);
        }
        ServerConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
            AbstractConnectionProcess(hconn,err){}
        ~ServerConnectionProcess() = default;
        virtual bool requestable() const noexcept override{
            return false;
        }
    };
}
