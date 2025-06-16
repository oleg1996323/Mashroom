#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    namespace client{

    namespace detail{
        class MessageHandler;
    }

    template<>
    struct AssociatedData<TYPE_MESSAGE::DATA_REQUEST>{
        ErrorCode err_ = ErrorCode::NONE;
    };
    
    template<>
    class Message<TYPE_MESSAGE::DATA_REQUEST>:__Message__<TYPE_MESSAGE::DATA_REQUEST>{
        std::optional<TimeFrame> t_fcst_;
        std::optional<uint16_t> parameter_sz_;
        std::optional<Organization> center;
        std::optional<utc_tp> from;   //requested from Date
        std::optional<utc_tp> to;     //requested to Date
        std::optional<Coord> pos;   //requested position (in WGS)
        std::optional<RepresentationType> rep_t;
        std::optional<TimeOffset> time_off_;
        std::optional<Extract::ExtractFormat> fmt_ = Extract::ExtractFormat::TXT_F|Extract::ExtractFormat::ARCHIVED;

    };
    }
}