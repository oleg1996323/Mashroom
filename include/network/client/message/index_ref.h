#pragma once
#include "network/common/message/msgdef.h"
#include "types/time_interval.h"
#include "API/grib1/include/sections/grid/grid.h"

namespace network{
    template<>
    struct MessageAdditional<network::Client_MsgT::INDEX_REF>{
        std::optional<RepresentationType> grid_type_;
        std::optional<TimeInterval> time_;
        std::optional<TimeFrame> forecast_preference_;
        utc_tp last_update_;
        public:
        MessageAdditional() = default;
        MessageAdditional(const MessageAdditional& other) = delete;
        MessageAdditional(MessageAdditional&& other) noexcept{
            *this = std::move(other);
        }
        MessageAdditional(const TimeInterval& tinterval,utc_tp last_update):
        time_(tinterval),last_update_(last_update){}
        MessageAdditional& operator=(const MessageAdditional& other) = delete;
        MessageAdditional& operator=(MessageAdditional&& other) noexcept{
            if(this!=&other){
                time_ = std::move(other.time_);
                last_update_ = std::move(other.last_update_);
            }
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>{
        using type = MessageAdditional<network::Client_MsgT::INDEX_REF>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.grid_type_,msg.time_,msg.forecast_preference_,msg.last_update_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>{
        using type = MessageAdditional<network::Client_MsgT::INDEX_REF>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.grid_type_,msg.time_,msg.forecast_preference_,msg.last_update_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::INDEX_REF>>{
        using type = MessageAdditional<network::Client_MsgT::INDEX_REF>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.grid_type_,msg.time_,msg.forecast_preference_,msg.last_update_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::INDEX_REF>>{
        using type = MessageAdditional<network::Client_MsgT::INDEX_REF>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::grid_type_),decltype(type::time_),
            decltype(type::forecast_preference_),decltype(type::last_update_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::INDEX_REF>>{
        using type = MessageAdditional<network::Client_MsgT::INDEX_REF>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::grid_type_),decltype(type::time_),
            decltype(type::forecast_preference_),decltype(type::last_update_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);
static_assert(std::is_move_constructible_v<network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);
static_assert(std::is_move_assignable_v<network::MessageAdditional<network::Client_MsgT::INDEX_REF>>);