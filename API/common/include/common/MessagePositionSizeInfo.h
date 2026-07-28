#pragma once
#include <cstddef>
#include "OsterLib/serialization.h"

struct MessagePositionSizeInfo{
    size_t begin_;
    size_t size_;

    bool operator==(const MessagePositionSizeInfo& other) const noexcept{
        return begin_==other.begin_ && size_==other.size_;
    }
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessagePositionSizeInfo>{
        using type = MessagePositionSizeInfo;
        SerializationEC operator()(const type& msg,
            std::vector<char>& buf) const noexcept{
                return serialize<NETWORK_ORDER>(msg,
                    buf,
                    msg.begin_,
                    msg.size_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessagePositionSizeInfo>{
        using type = MessagePositionSizeInfo;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,
                    buf,
                    msg.begin_,
                    msg.size_);
        }
    };

    template<>
    struct Serial_size<MessagePositionSizeInfo>{
        using type = MessagePositionSizeInfo;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                    msg.begin_,
                    msg.size_);
        }
    };

    template<>
    struct Min_serial_size<MessagePositionSizeInfo>{
        using type = MessagePositionSizeInfo;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<
                decltype(type::begin_),
                decltype(type::size_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessagePositionSizeInfo>{
        using type = MessagePositionSizeInfo;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<
                decltype(type::begin_),
                decltype(type::size_)>();
        }();
    };
}