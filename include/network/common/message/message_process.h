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
        MessageHandler<sent_from<S>()> recv_hmsg_;
        ErrorCode err_;
        template<auto T>
        requires MessageEnumConcept<T>
        ErrorCode __send__(Socket sock) noexcept{
            if(!hmsg_.has_message())
                return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"not message",AT_ERROR_ACTION::CONTINUE);
            auto& msg_tmp = hmsg_.template get<T>();
            if(serialization::SerializationEC err = serialization::serialize_network(msg_tmp,msg_tmp.buffer());
                    err!=serialization::SerializationEC::NONE)
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"serialization",AT_ERROR_ACTION::CONTINUE);
            return send(sock,msg_tmp.buffer());
        }
        template<auto MSG_T>
        requires MessageEnumConcept<MSG_T>
        ErrorCode __receive_and_define_message__(Socket sock, std::vector<char>& buffer) noexcept;
        ErrorCode __receive_and_define_any_message__(Socket sock, std::vector<char>& buffer) noexcept;
        public:

        template<MESSAGE_ID<S>::type MSG_T>
        std::optional<std::reference_wrapper<const Message<MSG_T>>> get_sending_message() const{
            if(std::holds_alternative<Message<MSG_T>>(hmsg_))
                return std::cref(std::get<Message<MSG_T>>(hmsg_));
            else return std::nullopt;
        };

        template<MESSAGE_ID<sent_from<S>()>::type MSG_T>
        std::optional<std::reference_wrapper<const Message<MSG_T>>> get_received_message() const{
            if(std::holds_alternative<Message<MSG_T>>(recv_hmsg_))
                return std::cref(std::get<Message<MSG_T>>(recv_hmsg_));
            else return std::nullopt;
        };

        template<MESSAGE_ID<S>::type T,typename... ARGS>
        ErrorCode send_message(Socket sock,ARGS... args){
            if(ErrorCode err = hmsg_.template emplace_message<T>(std::forward<ARGS>(args)...);err!=ErrorCode::NONE){
                hmsg_.clear();
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"creation error message",AT_ERROR_ACTION::CONTINUE);
            }
            else
                return __send__<T>(sock);
        }

        /**
         * @brief Common method for message receiving from other side.
         * @return ErrorCode resulting this function
         */
        template<MESSAGE_ID<sent_from<S>()>::type MSG_T>
        requires MessageEnumConcept<MSG_T>
        ErrorCode receive_message(Socket sock){
            std::vector<char> buffer;
            if(ErrorCode err = __receive_and_define_message__<MSG_T>(sock,buffer);
                err != ErrorCode::NONE)
                return err;
            else return ErrorCode::NONE;
        }

        ErrorCode receive_any_message(Socket sock){
            std::vector<char> buffer;
            if(ErrorCode err = __receive_and_define_any_message__(sock,buffer);
                err != ErrorCode::NONE)
                return err;
            else return ErrorCode::NONE;
        }

        std::optional<typename MESSAGE_ID<sent_from<S>()>::type> received_message_type() const{
            if(!recv_hmsg_.has_message())
                return std::nullopt;
            else return static_cast<typename MESSAGE_ID<sent_from<S>()>::type>(recv_hmsg_.index()-1);
        }
        std::optional<MESSAGE_ID<S>> sending_message_type() const{
            if(!recv_hmsg_.has_message())
                return std::nullopt;
            else return static_cast<MESSAGE_ID<S>>(hmsg_.index()-1);
        }
    };
    template<Side S>
    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    inline ErrorCode MessageProcess<S>::__receive_and_define_message__(Socket sock, std::vector<char>& buffer) noexcept{
        buffer.resize(MessageBase<MSG_T>::min_required_defining_size());
        if(ErrorCode err = receive(sock,buffer,MessageBase<MSG_T>::min_required_defining_size());
            err != ErrorCode::NONE)
            return err;
        else{
            if(auto res = Message<MSG_T>::template data_size_from_buffer<true>(std::span<const char>(buffer));!res.has_value())
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message",AT_ERROR_ACTION::CONTINUE);
            else{
                buffer.resize(res.value());
                if(ErrorCode err = receive(sock,buffer,buffer.size());
                    err != ErrorCode::NONE){
                    return err;
                }
                else {
                    serialization::SerializationEC code = serialization::deserialize_network(recv_hmsg_,std::span<const char>(buffer));
                    if(code!=serialization::SerializationEC::NONE)
                        return ErrorCode::DESERIALIZATION_ERROR;
                    else return ErrorCode::NONE;
                }
            }
        }
    }

    template<Side S>
    inline ErrorCode MessageProcess<S>::__receive_and_define_any_message__(Socket sock, std::vector<char>& buffer) noexcept{
        buffer.resize(undefined_msg_type_min_required_size<S>());
        if(ErrorCode err = receive(sock,buffer,undefined_msg_type_min_required_size<S>());
            err != ErrorCode::NONE)
            return err;
        else{
            auto msg_t_tmp = message_type_from_buffer<S,true>(std::span<const char>(buffer));
            if(!msg_t_tmp.has_value())
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message type",AT_ERROR_ACTION::CONTINUE);
            if(auto res = recv_hmsg_.emplace_default_message_by_id(size_t(msg_t_tmp.value()));res!=ErrorCode::NONE)
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message",AT_ERROR_ACTION::CONTINUE);
            if(auto res = recv_hmsg_.template data_size_from_buffer<true>(std::span<const char>(buffer));!res.has_value()){
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message",AT_ERROR_ACTION::CONTINUE);
            }
            else {
                buffer.resize(res.value());
                if(ErrorCode err = receive(sock,buffer,buffer.size());
                    err != ErrorCode::NONE){
                    return err;
                }
            }
            serialization::SerializationEC code = serialization::deserialize_network(recv_hmsg_,std::span<const char>(buffer));
            if(code!=serialization::SerializationEC::NONE)
                return ErrorCode::DESERIALIZATION_ERROR;
            else return ErrorCode::NONE;
        }
    }

    // /// @brief 
    // /// @param sock socket
    // /// @param status server status
    // /// @param err error code
    // /// @return 
    // template<>
    // template<>
    // inline ErrorCode MessageProcess<Side::SERVER>::send_message<network::Server_MsgT::ERROR>(Socket sock,ErrorCode err,server::Status status){
    //     if(ErrorCode err = hmsg_.emplace_message<network::Server_MsgT::ERROR>(err_);err!=ErrorCode::NONE){
    //         hmsg_.clear();
    //         return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"creation error message",AT_ERROR_ACTION::CONTINUE);
    //     }
    //     else
    //         return __send__<network::Server_MsgT::ERROR>(sock);
    // }

    // /// @brief 
    // /// @param sock socket
    // /// @param status server status
    // /// @param err error code
    // /// @return 
    // template<>
    // template<>
    // inline ErrorCode MessageProcess<Side::SERVER>::send_message<network::Server_MsgT::SERVER_STATUS>(Socket sock,server::Status status){
    //     if(ErrorCode err = hmsg_.emplace_message<network::Server_MsgT::SERVER_STATUS>(status);err!=ErrorCode::NONE){
    //         hmsg_.clear();
    //         return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"creation error message",AT_ERROR_ACTION::CONTINUE);
    //     }
    //     else return __send__<network::Server_MsgT::SERVER_STATUS>(sock);
    // }

    // template<>
    // template<>
    // inline ErrorCode MessageProcess<Side::CLIENT>::send_message<network::Client_MsgT::TRANSACTION>(Socket sock, Transaction transaction)
    // {
    //     err_ = hmsg_.emplace_message<network::Client_MsgT::TRANSACTION>(transaction);
    //     if(err_!=ErrorCode::NONE){
    //         hmsg_.clear();
    //         return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"creation error message",AT_ERROR_ACTION::CONTINUE);
    //     }
    //     else return __send__<network::Client_MsgT::TRANSACTION>(sock);
    // }

    template<>
    template<>
    inline ErrorCode MessageProcess<Side::SERVER>::send_message<network::Server_MsgT::DATA_REPLY_FILEINFO>(
                Socket sock,
                server::Status status,
                const fs::path& path,
                uint64_t offset,
                uint64_t size)
    {
        if(err_ = hmsg_.emplace_message<network::Server_MsgT::DATA_REPLY_FILEINFO>(status,path,offset,size);err_!=ErrorCode::NONE)
            return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        auto& msg = hmsg_.get<network::Server_MsgT::DATA_REPLY_FILEINFO>();
        serialization::SerializationEC code = serialization::serialize_network(msg,msg.buffer());
        if(err_ = send(sock,std::span<const char>(msg.buffer())); err_!=ErrorCode::NONE){
            if(err_ = send_message<network::Server_MsgT::ERROR>(sock,err_,server::Status::READY); err_!=ErrorCode::NONE)
                close(sock);
            return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
    }

    // /// @brief 
    // /// @details Send header with file information. If accepted by client, the file will be sent by parts.
    // /// @param sock socket
    // /// @param status server status
    // /// @param chunk part of file data by message
    // /// @param path path destination of file
    // /// @return success or failure of sending
    // template<>
    // template<>
    // inline ErrorCode MessageProcess<Side::SERVER>::send_message<network::Server_MsgT::DATA_REPLY_FILEPART>(
    //                 Socket sock,
    //                 server::Status status,
    //                 uint32_t chunk,
    //                 const fs::path& path,
    //                 uint64_t offset,
    //                 uint64_t size)
    // {
    //     if(err_ = send_message<network::Server_MsgT::DATA_REPLY_FILEINFO>(sock,status,path,offset,size); err_!=ErrorCode::NONE){
    //         if(err_ = send_message<network::Server_MsgT::ERROR>(sock,err_,server::Status::READY); err_!=ErrorCode::NONE)
    //             close(sock);
    //         return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
    //     }
    //     if(err_ = receive_message<network::Client_MsgT::TRANSACTION>(sock);err_!=ErrorCode::NONE){
    //         if(err_ = send_message<network::Server_MsgT::ERROR>(sock,err_,server::Status::READY); err_!=ErrorCode::NONE)
    //             close(sock);
    //         return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
    //     }
    //     assert(recv_hmsg_.msg_type() == Client_MsgT::TRANSACTION);
    //     auto& transaction_ = recv_hmsg_.get<network::Client_MsgT::TRANSACTION>();
    //     if(transaction_.additional_.op_status_==Transaction::ACCEPT){
    //         SendingFileInstance Ifile(path,chunk,offset);
    //         if(Ifile.err_!=ErrorCode::NONE){
    //             if(err_ = send_message<network::Server_MsgT::ERROR>(sock,err_,server::Status::READY); err_!=ErrorCode::NONE)
    //                 close(sock);
    //             return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
    //         }
    //         else{
    //             for(;next_chunk(Ifile) && Ifile.err_==ErrorCode::NONE;){
    //                 //previously send file-part info
    //                 send(sock,std::span<const char>(Ifile.from_mapping_,Ifile.from_mapping_+Ifile.chunk_));
    //             }
    //             if(Ifile.err_!=ErrorCode::NONE){
    //                 if(err_ = send_message<network::Server_MsgT::ERROR>(sock,err_,server::Status::READY); err_!=ErrorCode::NONE)
    //                     close(sock);
    //                 return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
    //             }
    //             else return ErrorCode::NONE;
    //         }
    //     }
    //     else return ErrorPrint::print_error(ErrorCode::TRANSACTION_REFUSED,"",AT_ERROR_ACTION::CONTINUE);
    // }
}