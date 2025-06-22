#pragma once
#include "message.h"
#include "message_handler.h"
#include "network/common/trans/send.h"
#include "network/common/trans/receive.h"
#include <cassert>

namespace network{
    template<Side S>
    class MessageProcess{
        template<Side ASide>
        friend class MessageProcess;
        MessageHandler<S> hmsg_;
        ErrorCode err_;
        bool __deserialize_msg__(std::span<const char> buffer);
        bool __receive_buffer__(int sock, std::vector<char>& buffer);
        public:
        bool receive(int sock){
            std::vector<char> buffer;
            if(__receive_buffer__(sock,buffer))
                if(__deserialize_msg__(std::move(buffer)))
                    return true;
            return false;
        }

        std::optional<Server_MsgT::type> get_msg_type() const{
            if(std::holds_alternative<std::monostate>(hmsg_))
                return std::nullopt;
            else return (Server_MsgT::type)hmsg_.index();
        }

        // bool send_file_by_parts(int sock,server::Status status,uint32_t chunk,const fs::path& filename);
        // bool send_error(int sock,server::Status status,ErrorCode err);
        template<MESSAGE_ID<S>::type,typename... ARGS>
        ErrorCode send_message(int sock,ARGS... args);
        template<MESSAGE_ID<S>::type>
        ErrorCode receive_message(int sock);
    };

    /// @brief 
    /// @details Send header with file information. If accepted by client, the file will be sent by parts.
    /// @param sock socket
    /// @param status server status
    /// @param chunk part of file data by message
    /// @param path path destination of file
    /// @return success or failure of sending
    template<>
    template<>
    ErrorCode MessageProcess<Side::SERVER>::send_message<Server_MsgT::DATA_REPLY_FILEPART>(
                    int sock,
                    server::Status status,
                    uint32_t chunk,
                    const fs::path& path,
                    uint64_t offset,
                    uint64_t size)
    {
        if(err_ = send_message<Server_MsgT::DATA_REPLY_FILEINFO>(sock,status,path,offset,size); err_!=ErrorCode::NONE){
            if(err_ = send_message<Server_MsgT::ERROR>(sock,server::Status::READY,err_); err_!=ErrorCode::NONE)
                close(sock);
            return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
        MessageProcess<Side::CLIENT> accept_decline;
        if(err_ = accept_decline.receive_message<Client_MsgT::TRANSACTION>(sock);err_!=ErrorCode::NONE){
            if(err_ = send_message<Server_MsgT::ERROR>(sock,server::Status::READY,err_); err_!=ErrorCode::NONE)
                close(sock);
            return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
        assert(accept_decline.hmsg_.msg_type() == Client_MsgT::TRANSACTION);
        auto& transaction_ = accept_decline.hmsg_.get<Client_MsgT::TRANSACTION>();
        if(transaction_.additional_.op_status_==Transaction::ACCEPT){
            SendingFileInstance Ifile(path,chunk,offset);
            if(Ifile.err_!=ErrorCode::NONE){
                if(err_ = send_message<Server_MsgT::ERROR>(sock,server::Status::READY,err_); err_!=ErrorCode::NONE)
                    close(sock);
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
            }
            else{
                for(;next_chunk(Ifile) && Ifile.err_==ErrorCode::NONE;){
                    //previously send file-part info
                    send(sock,std::span<const char>(Ifile.from_mapping_,Ifile.from_mapping_+Ifile.chunk_));
                }
                if(Ifile.err_!=ErrorCode::NONE){
                    if(err_ = send_message<Server_MsgT::ERROR>(sock,server::Status::READY,err_); err_!=ErrorCode::NONE)
                        close(sock);
                    return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
                }
                else return ErrorCode::NONE;
            }
        }
        else return ErrorPrint::print_error(ErrorCode::TRANSACTION_REFUSED,"",AT_ERROR_ACTION::CONTINUE);
    }

    template<>
    template<>
    ErrorCode MessageProcess<Side::SERVER>::send_message<Server_MsgT::DATA_REPLY_FILEINFO>(
                int sock,
                server::Status status,
                const fs::path& path,
                uint64_t offset,
                uint64_t size)
    {
        if(err_ = hmsg_.create_message<Server_MsgT::DATA_REPLY_FILEINFO>(path,status);err_!=ErrorCode::NONE)
            return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        auto& msg = hmsg_.get<Server_MsgT::DATA_REPLY_FILEINFO>();
        serialization::SerializationEC code = serialization::serialize_network(msg,msg.__buffer__());
        if(err_ = send(sock,std::span<const char>(msg.buffer())); err_!=ErrorCode::NONE){
            if(err_ = send_message<Server_MsgT::ERROR>(sock,server::Status::READY,err_); err_!=ErrorCode::NONE)
                close(sock);
            return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
    }

    /// @brief 
    /// @param sock socket
    /// @param status server status
    /// @param err error code
    /// @return 
    template<>
    template<>
    ErrorCode MessageProcess<Side::SERVER>::send_message<Server_MsgT::ERROR>(int sock,server::Status status,ErrorCode err){
        return false;
    }
}