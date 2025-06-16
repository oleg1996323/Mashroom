#pragma once
#include <network/common/message/msgdef.h>

namespace network::server{
template<>
class Message<TYPE_MESSAGE::SERVER_STATUS>:public __Message__<TYPE_MESSAGE::SERVER_STATUS>{
    Message(server::Status status):
    __Message__<TYPE_MESSAGE::SERVER_STATUS>(status,0){}
    static void serialize_impl(const Message<type_msg_>& msg){

    }
    static Message<type_msg_> deserialize_impl(const std::vector<char>& buf){
        if(!buf.empty())
            return Message(get_status(buf));
    }
};
}