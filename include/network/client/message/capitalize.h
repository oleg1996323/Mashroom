#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    namespace client{

    namespace detail{
        class MessageHandler;
    }

    template<>
    struct AssociatedData<TYPE_MESSAGE::CAPITALIZE>{
        ErrorCode err_ = ErrorCode::NONE;
    };
    
    template<>
    struct Message<TYPE_MESSAGE::CAPITALIZE>:__Message__<TYPE_MESSAGE::CAPITALIZE>{};
    }
}