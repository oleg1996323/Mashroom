#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    namespace client{
    namespace detail{
        class MessageHandler;
    }

    template<>
    struct AssociatedData<TYPE_MESSAGE::CAPITALIZE_REF>{
        ErrorCode err_ = ErrorCode::NONE;
    };

    template<>
    class Message<TYPE_MESSAGE::CAPITALIZE_REF>:__Message__<TYPE_MESSAGE::CAPITALIZE_REF>{
        Message():__Message__<TYPE_MESSAGE::CAPITALIZE_REF>(0){}
    };
    }
}