#include "network/server/message/data_reply_extract_part.h"

namespace network::server{
    Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>::Message(ProgressBase progr,const fs::path& file_path, server::Status status):
    __Message__<TYPE_MESSAGE::DATA_REPLY_EXTRACT>(status,sizeof(fn_sz)+fn_sz+sizeof(file_sz)+sizeof(ProgressBase)){

    }
    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>::serialize_impl() const{
        
    }
    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>::deserialize_impl(){
        auto& buf = __buffer__();
        if(buf.size()==sizeof(fn_sz)+fn_sz+sizeof(file_sz)+sizeof(file_hash_)){
            
        }
        else return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"invalid message size",AT_ERROR_ACTION::CONTINUE);
        if(buf.size()==sizeof(Message<type_msg_>)){
            size_t filename_size_tmp = *(size_t*)buf.data()+base_msg_sz<type_msg_>;
            return Message<type_msg_>(      //progress
                                            *(ProgressBase*)(buf.data()+
                                            base_msg_sz<type_msg_>+
                                            sizeof(fn_sz)+
                                            filename_size_tmp+
                                            sizeof(file_sz)),
                                            //filename
                                            std::string(buf.data()+
                                            base_msg_sz<type_msg_>+
                                            sizeof(fn_sz),
                                            buf.data()+
                                            base_msg_sz<type_msg_>+
                                            sizeof(fn_sz)+filename_size_tmp),
                                            //server status
                                            *(server::Status*)(buf.data()+sizeof(TYPE_MESSAGE)));
        }
    }
}