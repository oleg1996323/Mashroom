#pragma once
#include "message.h"
#include "message_handler.h"
#include "send.h"
#include "receive.h"
#include <cassert>
#include "commonsocket.h"

namespace network{

    template<Side S, bool NETWORK_ORDER>
    std::expected<Message_t<S>,serialization::SerializationEC> message_type_from_buffer(std::span<const char> buffer) noexcept{
        using namespace serialization;
        using id_t = size_t;//std::invoke_result_t<decltype(&MessageHandler<S>::index), MessageHandler<S>>;
        assert(buffer.size()>=sizeof(id_t));
        id_t result;
        if(SerializationEC code = deserialize<NETWORK_ORDER>(result,buffer);code!=SerializationEC::NONE)
            return std::unexpected(code);
        if(result+1>=MESSAGE_ID<S>::count())
            return std::unexpected(serialization::SerializationEC::UNMATCHED_TYPE);
        else return static_cast<Message_t<S>>(result+1);
    }

    template<Side S>
    class MessageProcess{
        template<Side ASide>
        friend class MessageProcess;
        MessageHandler<S> hmsg_;
        MessageHandler<sent_from<S>()> recv_hmsg_;
        ErrorCode err_ = ErrorCode::NONE;
        public:
        MessageProcess() = default;
        MessageProcess(const MessageProcess<S>& other) = delete;
        MessageProcess(MessageProcess<S>&& other) noexcept{
            *this=std::move(other);
        }
        ~MessageProcess(){
            std::cout<<"MessageProcess deleted"<<std::endl;
        }
        MessageProcess<S>& operator=(const MessageProcess<S>&) = delete;
        MessageProcess<S>& operator=(MessageProcess<S>&& other) noexcept{
            if(this!=&other){
                //hmsg_.swap(other.hmsg_);
            }
            return *this;
        }
        private:
        template<auto T>
        requires MessageEnumConcept<T>
        ErrorCode __send__(const Socket& sock) noexcept{
            hmsg_.buffer().clear();
            if(!hmsg_.has_message())
                return ErrorPrint::print_error(ErrorCode::SENDING_MESSAGE_ERROR,"not message",AT_ERROR_ACTION::CONTINUE);
            if(serialization::SerializationEC err = serialization::serialize_network(static_cast<uint64_t>(serialization::serial_size(hmsg_)),hmsg_.buffer());
                    err!=serialization::SerializationEC::NONE)
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"serialization",AT_ERROR_ACTION::CONTINUE);
            std::cout<<serialization::serial_size(hmsg_)<<" data_size"<<std::endl;
            if(serialization::SerializationEC err = serialization::serialize_network(hmsg_,hmsg_.buffer());
                    err!=serialization::SerializationEC::NONE)
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"serialization",AT_ERROR_ACTION::CONTINUE);
            auto err = send(sock,hmsg_.buffer())==-1?ErrorCode::SENDING_MESSAGE_ERROR:ErrorCode::NONE;
            if(hmsg_.buffer().size()>100){
                std::ofstream out("/home/oster/Mashroom/sent.txt",std::ofstream::trunc|std::ofstream::out);
                for(int i = 0;i<100;++i)
                    out<<static_cast<int>(hmsg_.buffer().at(i))<<" ";
                out.flush();
            }
            return err;
        }
        template<auto MSG_T>
        requires MessageEnumConcept<MSG_T>
        ErrorCode __receive_and_define_message__(const Socket& sock) noexcept;
        ErrorCode __receive_and_define_any_message__(const Socket& sock) noexcept;
        public:

        template<MESSAGE_ID<S>::type MSG_T>
        const Message<MSG_T>& get_sending_message() const{
            if(std::holds_alternative<Message<MSG_T>>(hmsg_))
                return std::get<Message<MSG_T>>(hmsg_);
            else throw std::runtime_error("Handler didn't sent messages");
        };

        template<MESSAGE_ID<sent_from<S>()>::type MSG_T>
        const Message<MSG_T>& get_received_message() const{
            if(std::holds_alternative<Message<MSG_T>>(recv_hmsg_))
                return std::get<Message<MSG_T>>(recv_hmsg_);
            else throw std::runtime_error("Handler didn't received messages");
        };

        template<MESSAGE_ID<S>::type T,typename... ARGS>
        ErrorCode send_message(Socket sock,ARGS... args) noexcept{
            hmsg_.template emplace_message<T>(std::forward<ARGS>(args)...);
            return __send__<T>(sock);
        }

        /**
         * @brief Common method for message receiving from other side.
         * @return ErrorCode resulting this function
         */
        template<MESSAGE_ID<sent_from<S>()>::type MSG_T>
        requires MessageEnumConcept<MSG_T>
        ErrorCode receive_message(const Socket& sock) noexcept{
            if(ErrorCode err = __receive_and_define_message__<MSG_T>(sock);
                err != ErrorCode::NONE)
                return err;
            else return ErrorCode::NONE;
        }

        ErrorCode receive_any_message(const Socket& sock) noexcept{
            if(ErrorCode err = __receive_and_define_any_message__(sock);
                err != ErrorCode::NONE)
                return err;
            else return ErrorCode::NONE;
        }

        std::optional<typename MESSAGE_ID<sent_from<S>()>::type> received_message_type() const noexcept{
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
    inline ErrorCode MessageProcess<S>::__receive_and_define_message__(const Socket& sock) noexcept{
        std::vector<char> buffer;
        buffer.clear();
        buffer.resize(sizeof(uint64_t));
        if(receive(sock,buffer,buffer.size())==-1){
            recv_hmsg_.buffer().swap(buffer);
            return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
        else{
            if(auto res = Message<MSG_T>::template data_size_from_buffer<true>(std::span<const char>(buffer));!res.has_value()){
                recv_hmsg_.buffer().swap(buffer);
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message",AT_ERROR_ACTION::CONTINUE);
            }
            else{
                buffer.resize(res.value());
                if(receive(sock,buffer,buffer.size())){
                    recv_hmsg_.buffer().swap(buffer);
                    return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
                }
                else {
                    serialization::SerializationEC code = serialization::deserialize_network(recv_hmsg_,std::span<const char>(buffer));
                    if(code!=serialization::SerializationEC::NONE){
                        recv_hmsg_.buffer().swap(buffer);
                        return ErrorCode::DESERIALIZATION_ERROR;
                    }
                    else{
                        recv_hmsg_.buffer().swap(buffer);
                        return ErrorCode::NONE;
                    }
                }
            }
        }
        
    }

    template<Side S>
    inline ErrorCode MessageProcess<S>::__receive_and_define_any_message__(const Socket& sock) noexcept{
        using namespace serialization;
        std::vector<char> buffer;
        buffer.resize(sizeof(uint64_t));
        if(receive(sock,buffer,buffer.size())==-1){
            recv_hmsg_.buffer().swap(buffer);
            return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        }
        else{
            uint64_t data_sz=0;
            if(serialization::deserialize_network(data_sz,std::span<const char>(buffer))!=serialization::SerializationEC::NONE){
                recv_hmsg_.buffer().swap(buffer);
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message. Cannot define message size",AT_ERROR_ACTION::CONTINUE);
            }
            std::cout<<"Buffer increased by "<<data_sz<<std::endl;
            buffer.clear();
            buffer.resize(data_sz,0);
            if(receive(sock,buffer,buffer.size())==-1){
                recv_hmsg_.buffer().swap(buffer);
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"",AT_ERROR_ACTION::CONTINUE);
            }
            if(buffer.size()>100){
                std::ofstream out("/home/oster/Mashroom/recv.txt",std::ofstream::trunc|std::ofstream::out);
                for(int i = 0;i<100;++i)
                    out<<static_cast<int>(buffer.at(i))<<" ";
                out.flush();
            }
            if(serialization::deserialize_network(recv_hmsg_,std::span<const char>(buffer))!=serialization::SerializationEC::NONE){
                recv_hmsg_.buffer().swap(buffer);
                return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message. Cannot define message type/structure",AT_ERROR_ACTION::CONTINUE);
            }
            else{
                recv_hmsg_.buffer().swap(buffer);
                return ErrorCode::NONE;
            }
        }
    }
}