#pragma once
#include "msgdef.h"

namespace network::server{
template<>
class Message<TYPE_MESSAGE::VERSION>:public __Message__<TYPE_MESSAGE::VERSION>{
    uint64_t version_;
    static void serialize_impl(const Message<type_msg_>& msg){
        msg.buffer().insert(msg.buffer().end(),(char*)&msg,(char*)(&msg+base_msg_sz<type_msg_>));
    }
    static Message<type_msg_> deserialize_impl(const std::vector<char>& buf){
        
    }
};
}