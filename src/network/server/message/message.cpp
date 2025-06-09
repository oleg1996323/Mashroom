#include "network/server/message/message.h"

namespace network::server{
    bool MessageProcess::__deserialize_msg__(std::vector<char>&& buffer){
        if(!buffer.empty()){
            if((int)(*(TYPE_MESSAGE*)(buffer.data()+sizeof(uint64_t)))>=(std::variant_size_v<reply_message>-1)){
                hmsg_=std::monostate();
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"invalid message type",AT_ERROR_ACTION::CONTINUE);
                return false;
            }
            switch((TYPE_MESSAGE)buffer.at(0)){
                case TYPE_MESSAGE::DATA_REPLY_FILEINFO:
                    hmsg_ = Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::SERVER_STATUS:
                    hmsg_ = Message<TYPE_MESSAGE::SERVER_STATUS>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::DATA_REPLY_CAPITALIZE:
                    hmsg_ = Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::ERROR:
                    hmsg_ = Message<TYPE_MESSAGE::ERROR>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::PROGRESS:
                    hmsg_ = Message<TYPE_MESSAGE::PROGRESS>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::DATA_REPLY_FILEPART:
                    hmsg_ = Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::VERSION:
                    hmsg_ = Message<TYPE_MESSAGE::VERSION>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_REF:
                    hmsg_ = Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_REF>::deserialize(std::move(buffer));
                case TYPE_MESSAGE::DATA_REPLY_EXTRACT:{
                    hmsg_ = Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>::deserialize(std::move(buffer));
                }
                default:{
                    hmsg_ = std::monostate();
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"unknown message",AT_ERROR_ACTION::CONTINUE);
                    return false;
                }
            }
        }
        else {
            hmsg_ = std::monostate();
            hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"empty message",AT_ERROR_ACTION::CONTINUE);
            return false;
        }
    }

    bool MessageProcess::__receive_buffer__(int sock, std::vector<char>& buffer){
        buffer.clear();
        buffer.resize(base_msg_sz<(TYPE_MESSAGE)0>);
        if(recv(sock,buffer.data(),base_msg_sz<(TYPE_MESSAGE)0>,MSG_WAITALL)!=base_msg_sz<(TYPE_MESSAGE)0>){
            if(errno!=0){
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                errno=0;
                return false;
            }
            else{
                hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"nothing to read",AT_ERROR_ACTION::CONTINUE);
                return false;
            }
        }
        if((int)(*(TYPE_MESSAGE*)(buffer.data()+sizeof(uint64_t)))>=(std::variant_size_v<reply_message>-1)){
            hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"invalid message type",AT_ERROR_ACTION::CONTINUE);
            return false;
        }
        uint64_t to_read = *(uint64_t*)buffer.data();
        if(to_read==0)
            return true;
        buffer.resize(to_read);
        uint64_t cur_buf_pos = base_msg_sz<(TYPE_MESSAGE)0>;
        while(to_read>0){
            int64_t read_tmp = recv(sock,(void*)buffer.data()+cur_buf_pos,to_read,MSG_WAITALL);
            if(read_tmp<0){
                if(errno!=0){
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                    errno=0;
                    return false;
                }
                else{
                    hmsg_.err_ = ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"nothing to read",AT_ERROR_ACTION::CONTINUE);
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