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
        MessageHandler<Side::CLIENT> recv_hmsg_;
        MessageHandler<Side::SERVER> send_hmsg_;
        std::optional<size_t> version_; //@todo
        std::optional<Data_a> access_;
        std::optional<Client_MsgT> waiting_;
        void __index_process__(
                std::stop_token token,ClientAppMsg msg) noexcept;
        void __task__(std::error_code& err, network::Client_MsgT::type msg_id) noexcept;
        void __enqueue_error__(std::error_code& err,
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
            io_context().serialize(reply);
        }
        void __enqueue_error__(std::error_code& err,
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
