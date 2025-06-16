#pragma once
#include "progress_base.h"
#include <network/common/message/msgdef.h>

namespace network::server{
template<>
class Message<TYPE_MESSAGE::PROGRESS>:public __Message__<TYPE_MESSAGE::PROGRESS>{
    ProgressBase prog_;
    Message(const ProgressBase& progress,server::Status status):
    __Message__<TYPE_MESSAGE::PROGRESS>(status,sizeof(prog_)){}
    static void serialize_impl(const Message<type_msg_>& msg){

    }
    static Message<type_msg_> deserialize_impl(const std::vector<char>& buf){
        if(!buf.empty())
            return Message<TYPE_MESSAGE::PROGRESS>( *(ProgressBase*)(buf.data()+base_msg_sz<type_msg_>),
                                                    __Message__::get_status(buf));
    }
};
}