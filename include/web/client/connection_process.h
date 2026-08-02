#pragma once
#include "web/common/msgdef.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "OsterLib/network/abstractprocess.h"
#include "OsterLib/network/worker/command.h"
#include "web/error.h"

namespace network{
    class ClientConnectionProcess:public AbstractRequestableConnectionProcess{
        using recv_t = Frame<std::monostate,MessageHandler<Side::SERVER>,std::monostate>;
        using send_t = Frame<std::monostate,MessageHandler<Side::CLIENT>,std::monostate>;
        class ReceivingFileState{
            std::ofstream stream_;
            Message<Server_MsgT::FILE_METADATA> meta_;
            uint32_t has_received_=0;
            public:
            ReceivingFileState(
                Message<Server_MsgT::FILE_METADATA> meta) noexcept:
                meta_(meta)
            {
                if(fs::exists(meta_.filename()) || 
                    fs::is_regular_file(meta_.filename()))
                    stream_ = std::ofstream(meta_.filename());
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
            bool has_to_receive() const noexcept{
                return has_received_<meta_.file_size();
            }
            size_t size() const noexcept{
                return meta_.file_size();
            }
            size_t has_read() const noexcept{
                return has_received_;
            }
            void cancel() noexcept{
                meta_.state(Transaction::CANCEL);
            }
            bool canceled() const noexcept{
                return meta_.state()==Transaction::CANCEL;
            }
            std::error_code next(const Message<Server_MsgT::FILE_DATA>& msg) noexcept{
                if(has_to_receive()){
                    if((size()-has_received_)>=msg.data().size()){
                        stream_.write(msg.data().data(),msg.data().size());
                        return {};
                    }
                    else{
                        return std::make_error_code(std::errc::file_too_large);
                    }
                }
                else return std::make_error_code(
                            std::errc::operation_not_permitted);
            }
            float progress() const noexcept{
                return float(has_read())/size();
            }
        };
        
        MessageHandler<Side::SERVER> recv_hmsg_;
        MessageHandler<Side::CLIENT> send_hmsg_;
        std::optional<size_t> version_; //@todo
        std::unordered_map<std::string,ReceivingFileState> file_recv_;
        void __reaction__(
                std::error_code& err,
                Server_MsgT::type server_msg) noexcept;
        void __emplace_error__(std::error_code& err,
                std::string description,
                mashroom::network::errc code,
                std::vector<osterlib::Field> fields = {}) noexcept
        {
            Message<Client_MsgT::ERROR> reply(
                    code,
                    std::move(description));
            reply.add_fields(std::move(fields));
            io_context().send(err,[](const std::vector<char>&){},reply);
        }
        public:
        virtual ~ClientConnectionProcess() = default;
        
        virtual void on_read(std::error_code& err) noexcept override;

        ClientConnectionProcess(
                ConnectionHandle hconn,
                std::error_code& err) noexcept;
        virtual void on_write(std::error_code& err) noexcept override;
        virtual void on_task_done(std::error_code& err) noexcept override;
        virtual void on_stop_requested(std::error_code& err) noexcept override;
        virtual void on_push_request(std::error_code& err) noexcept override{
            err.clear();
            if(next_request()){
                using send_t = Frame<std::monostate,MessageHandler<Side::CLIENT>,std::monostate>;
                send_t* s;
                send_hmsg_ = active_request_->sent(s)->data_frame();
            }
            on_write(err);
        }
        virtual void on_init_connection(std::error_code& err) noexcept override;
    };
}