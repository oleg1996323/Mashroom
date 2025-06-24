#include "network/common/message/message_process.h"
#include "functional/serialization.h"

namespace network{
    bool MessageProcess::__deserialize_msg__(std::span<const char> buffer){
        if(!buffer.empty()){
            if((int)(*(Server_MsgT::type*)(buffer.data()+sizeof(uint64_t)))>=(std::variant_size_v<reply_message>-1)){
                hmsg_=std::monostate();
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message type",AT_ERROR_ACTION::CONTINUE);
                return false;
            }
            switch((Server_MsgT::type)buffer[0]){
                case Server_MsgT::DATA_REPLY_FILEINFO:{
                    Message<Server_MsgT::DATA_REPLY_FILEINFO> msg_tmp;
                    auto code = serialization::deserialize(msg_tmp,buffer);
                    hmsg_;
                }
                case Server_MsgT::SERVER_STATUS:
                    hmsg_ = Message<Server_MsgT::SERVER_STATUS>::deserialize(std::move(buffer));
                case Server_MsgT::DATA_REPLY_CAPITALIZE:
                    hmsg_ = Message<Server_MsgT::DATA_REPLY_CAPITALIZE>::deserialize(std::move(buffer));
                case Server_MsgT::ERROR:
                    hmsg_ = Message<Server_MsgT::ERROR>::deserialize(std::move(buffer));
                case Server_MsgT::PROGRESS:
                    hmsg_ = Message<Server_MsgT::PROGRESS>::deserialize(std::move(buffer));
                case Server_MsgT::DATA_REPLY_FILEPART:
                    hmsg_ = Message<Server_MsgT::DATA_REPLY_FILEPART>::deserialize(std::move(buffer));
                case Server_MsgT::VERSION:
                    hmsg_ = Message<Server_MsgT::VERSION>::deserialize(std::move(buffer));
                case Server_MsgT::DATA_REPLY_CAPITALIZE_REF:
                    hmsg_ = Message<Server_MsgT::DATA_REPLY_CAPITALIZE_REF>::deserialize(std::move(buffer));
                case Server_MsgT::DATA_REPLY_EXTRACT:{
                    hmsg_ = Message<Server_MsgT::DATA_REPLY_EXTRACT>::deserialize(std::move(buffer));
                }
                default:{
                    hmsg_ = std::monostate();
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"unknown message",AT_ERROR_ACTION::CONTINUE);
                    return false;
                }
            }
        }
        else {
            hmsg_ = std::monostate();
            hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"empty message",AT_ERROR_ACTION::CONTINUE);
            return false;
        }
    }

    bool MessageProcess::__receive_buffer__(int sock, std::vector<char>& buffer){
        buffer.clear();
        buffer.resize(base_msg_sz<(Server_MsgT)0>);
        if(recv(sock,buffer.data(),base_msg_sz<(Server_MsgT)0>,MSG_WAITALL)!=base_msg_sz<(Server_MsgT)0>){
            if(errno!=0){
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                errno=0;
                return false;
            }
            else{
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"nothing to read",AT_ERROR_ACTION::CONTINUE);
                return false;
            }
        }
        if((int)(*(Server_MsgT*)(buffer.data()+sizeof(uint64_t)))>=(std::variant_size_v<reply_message>-1)){
            hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"invalid message type",AT_ERROR_ACTION::CONTINUE);
            return false;
        }
        uint64_t to_read = *(uint64_t*)buffer.data();
        if(to_read==0)
            return true;
        buffer.resize(to_read);
        uint64_t cur_buf_pos = base_msg_sz<(Server_MsgT)0>;
        while(to_read>0){
            int64_t read_tmp = recv(sock,(void*)buffer.data()+cur_buf_pos,to_read,MSG_WAITALL);
            if(read_tmp<0){
                if(errno!=0){
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                    errno=0;
                    return false;
                }
                else{
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,"nothing to read",AT_ERROR_ACTION::CONTINUE);
                    return false;
                }
            }
            else if(read_tmp==0){
                continue;
            }
            else{
                to_read-=read_tmp;
                cur_buf_pos+=read_tmp;
            }
        }
        return true;
    }
}