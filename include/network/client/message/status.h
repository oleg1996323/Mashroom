#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    namespace client{

    namespace detail{
        class MessageHandler;
    }

    template<>
    struct AssociatedData<TYPE_MESSAGE::SERVER_STATUS>{
        ErrorCode err_ = ErrorCode::NONE;
    };
    
    template<>
    struct Message<TYPE_MESSAGE::SERVER_STATUS>:__Message__<TYPE_MESSAGE::SERVER_STATUS>{};
    }
}