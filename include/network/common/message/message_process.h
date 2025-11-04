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
        mutable std::unique_lock<std::mutex> locker_;
        std::atomic<bool> has_more_ = false;
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
                hmsg_ = std::move(other.hmsg_);
                recv_hmsg_ = std::move(other.recv_hmsg_);
                std::swap(locker_,other.locker_);
                //hmsg_.swap(other.hmsg_);
            }
            return *this;
        }
        void set_locker(std::unique_lock<std::mutex>&& locker) noexcept{
            locker_ = std::move(locker);
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
            auto err = send(sock,SEND_FLAGS::NoSignal,hmsg_.buffer())==-1?ErrorCode::SENDING_MESSAGE_ERROR:ErrorCode::NONE;
            if(hmsg_.buffer().size()>100){
                std::ofstream out("/home/oster/Mashroom/sent.txt",std::ofstream::trunc|std::ofstream::out);
                for(int i = 0;i<100;++i)
                    out<<static_cast<int>(hmsg_.buffer().at(i))<<" ";
                out.flush();
            }
            return err;
        }
        ErrorCode __receive_and_define_any_message__(const Socket& sock) noexcept;
        public:
        std::unique_lock<std::mutex>& locker() const{
            return locker_;
        }
        const std::atomic<bool>& has_more() const{
            return has_more_;
        }
        template<MESSAGE_ID<S>::type MSG_T>
        const Message<MSG_T>& get_sending_message() const{
            if(std::holds_alternative<Message<MSG_T>>(hmsg_)){
                return std::get<Message<MSG_T>>(hmsg_);
            }
            else{
                throw std::runtime_error("Handler didn't sent messages");
            }
        };

        template<MESSAGE_ID<sent_from<S>()>::type MSG_T>
        const Message<MSG_T>& get_received_message() const{
            if(std::holds_alternative<Message<MSG_T>>(recv_hmsg_)){
                return std::get<Message<MSG_T>>(recv_hmsg_);
            }
            else{
                throw std::runtime_error("Handler didn't received messages");
            }
        };

        template<MESSAGE_ID<S>::type T,typename... ARGS>
        ErrorCode send_message(Socket sock,ARGS... args) noexcept{
            hmsg_.template emplace_message<T>(std::forward<ARGS>(args)...);
            ErrorCode err = __send__<T>(sock);
            return err;

        }

        /**
         * @brief Common method for message receiving from other side.
         * @return ErrorCode resulting this function
         */
        ErrorCode receive_any_message(const Socket& sock) noexcept{
            if(ErrorCode err = __receive_and_define_any_message__(sock);
                err != ErrorCode::NONE){
                return err;
            }
            else{
                return ErrorCode::NONE;
            }
        }

        std::optional<typename MESSAGE_ID<sent_from<S>()>::type> received_message_type() const noexcept{
            if(!recv_hmsg_.has_message()){
                return std::nullopt;
            }
            else{
                return static_cast<typename MESSAGE_ID<sent_from<S>()>::type>(recv_hmsg_.index()-1);
            }
        }
        std::optional<MESSAGE_ID<S>> sending_message_type() const{
            if(!recv_hmsg_.has_message()){
                return std::nullopt;
            }
            else{
                return static_cast<MESSAGE_ID<S>>(hmsg_.index()-1);
            }
        }
    };

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
                auto has_more_msg = [this](auto&& msg){
                    if constexpr (std::is_same_v<std::decay_t<decltype(msg)>,std::monostate>)
                        this->has_more_ = false;
                    else this->has_more_.exchange(msg.msg_more_);
                };
                recv_hmsg_.buffer().swap(buffer);
                return ErrorCode::NONE;
            }
        }
    }
}