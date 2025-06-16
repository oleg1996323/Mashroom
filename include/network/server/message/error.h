#pragma once
#include <network/common/message/msgdef.h>

namespace network::server{
template<>
class Message<TYPE_MESSAGE::ERROR>:public __Message__<TYPE_MESSAGE::ERROR>{
    ErrorCode err_ = ErrorCode::NONE;
    Message(    ErrorCode err,
                server::Status status = server::Status::READY):
    __Message__<TYPE_MESSAGE::ERROR>(status,sizeof(err_)){}
    static void serialize_impl(const Message<type_msg_>& msg){
        msg.__buffer__().insert(msg.__buffer__().end(),&msg,&msg+sizeof(msg));
    }
    static Message<type_msg_> deserialize_impl(const std::vector<char>& buf){
        if(buf.size()==sizeof(Message<type_msg_>)){
            return Message<type_msg_>(  *(ErrorCode*)(buf.data()+base_msg_sz<type_msg_>),
                                        __Message__::get_status(buf));
        }
    }
};
}