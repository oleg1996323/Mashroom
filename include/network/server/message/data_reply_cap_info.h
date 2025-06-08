#pragma once
#include "msgdef.h"
#include "progress_base.h"

namespace network::server{
template<>
class Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_INFO>:public __Message__<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_INFO>{
    uint16_t fn_sz = 0;
    std::string filename;
    uintmax_t file_sz = 0;      //size of file
    Message(ProgressBase progr,const fs::path& file_path, server::Status status):
    __Message__<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_INFO>(status,sizeof(fn_sz)+fn_sz+sizeof(file_sz)+sizeof(ProgressBase)){

    }
    static void serialize_impl(const Message<type_msg_>& msg){
        
    }
    static Message<type_msg_> deserialize_impl(const std::vector<char>& buf){
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
};
}