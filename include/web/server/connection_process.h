#pragma once
#include "web/common/msgdef.h"
#include <string_view>
#include <memory>
#include <fstream>
#include <list>
#include <vector>

#include "network/abstractprocess.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "network/commonsocket.h"
#include "network/multiplexor/eventhandle.h"

namespace network{
    class ServerConnectionProcess:public AbstractConnectionProcess
    {
        class SendingFileState{
            std::ifstream stream_;
            Message<Server_MsgT::FILE_METADATA> meta_;
            uint32_t start_ = 0;
            uint32_t chunk_sz_=0;
            uint32_t has_read_=0;
            Transaction state_=Transaction::NEW;
            public:
            SendingFileState(
                Message<Server_MsgT::ERROR>& err,
                Message<Server_MsgT::FILE_METADATA> meta,
                uint32_t start,
                uint32_t chunk,
                size_t size) noexcept:
                meta_(meta),
                start_(start),
                chunk_sz_(chunk)
            {
                meta_.state(Transaction::NEW);
                if(fs::exists(meta_.filename()) || 
                    fs::is_regular_file(meta_.filename())){
                    stream_ = std::ifstream(meta_.filename());
                    stream_.seekg(start_,std::ios::beg);
                    if(stream_.fail()){
                        err.description("unexpected internal error");
                        err.error(ErrorCode::INTERNAL_ERROR);
                    }
                }
                else return;
            }
            const Message<Server_MsgT::FILE_METADATA>& meta() 
                const noexcept
            {
                return meta_;
            }
            bool valid() const noexcept{
                return stream_.is_open();
            }
            bool has_to_send() const noexcept{
                return has_read_<meta_.file_size();
            }
            size_t size() const noexcept{
                return meta_.file_size();
            }
            size_t chunk() const noexcept{
                return chunk_sz_;
            }
            size_t has_read() const noexcept{
                return has_read_;
            }
            size_t start_position() const noexcept{
                return start_;
            }
            bool accepted() const noexcept{
                return meta_.state()==Transaction::ACCEPT;
            }
            bool canceled() const noexcept{
                return meta_.state()==Transaction::CANCEL;
            }
            bool is_waiting_accept() const noexcept{
                return meta_.state()==Transaction::NEW;
            }
            std::error_code accept() noexcept{
                if(canceled())
                    return std::make_error_code(std::errc::operation_not_permitted);
                else if(accepted())
                    return std::make_error_code(std::errc::operation_not_permitted);
                else return std::error_code();
            }
            void cancel() noexcept{
                meta_.state(Transaction::CANCEL);
            }
            std::expected<Message<Server_MsgT::FILE_DATA>,std::error_code> next() noexcept{
                if(accepted() && has_to_send()){
                    Message<Server_MsgT::FILE_DATA> result(meta_.transaction());
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
        
        MessageHandler<Side::CLIENT> recv_hmsg_;
        MessageHandler<Side::SERVER> send_hmsg_;
        std::optional<size_t> version_; //@todo
        std::optional<Data_a> access_;
        std::optional<Client_MsgT> waiting_;
        std::unique_ptr<SendingFileState> file_sender_;
        void __task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept;
        void __emplace_error__(std::error_code& err,
                std::string description,
                server::Status status,
                ErrorCode code) noexcept
        {
            Message<Server_MsgT::ERROR> reply(
                    code,
                    ErrorPrint::message(
                        code,
                        std::move(description)),
                        status
                    );
            io_context().send(err,reply);
        }
        void __emplace_error__(std::error_code& err,
                std::string description,
                server::Status status,
                Message<Server_MsgT::TRANSACTION> transaction,
                ErrorCode code) noexcept
        {
            Message<Server_MsgT::ERROR> reply(
                    ErrorCode::INTERNAL_ERROR,
                    ErrorPrint::message(
                        code,
                        std::move(description)),
                        std::move(transaction),
                        status
                    );
            io_context().serialize(reply);
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
                ErrorCode::INTERNAL_ERROR);
        }
        ServerConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
            AbstractConnectionProcess(hconn,err){}
        ~ServerConnectionProcess() = default;
        virtual void handle_event(
                    Event event,
                    std::error_code& err) noexcept
        {
            
        }
        virtual bool requestable() const noexcept override{
            return false;
        }
    };
}
