#pragma once
#include <fstream>
#include "types/time_interval.h"
#include "serialization.h"
#include "sections/grid/grid.h"
#include "byte_read.h"
#include "types/time_interval.h"

struct SublimedDataInfo
{
    std::optional<GridInfo> grid_data_;
    std::vector<ptrdiff_t> buf_pos_;
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    utc_tp from_ = utc_tp::max();
    utc_tp to_ = utc_tp::min();
    std::chrono::system_clock::duration discret_ = std::chrono::system_clock::duration(0);
    // SublimedDataInfo(const std::optional<GridInfo>& grid_data,
    //                 const std::vector<ptrdiff_t>& buf_pos,
    //                 const utc_tp& from,
    //                 const utc_tp& to,
    //                 const std::chrono::system_clock::duration& discret):
    //                 grid_data_(grid_data_),
    //                 buf_pos_(buf_pos),
    //                 from_(from),
    //                 to_(to),
    //                 discret_
    // SublimedDataInfo() = default;
    // SublimedDataInfo(const SublimedDataInfo& other):
    // grid_data(other.grid_data_),
    // buf_pos_(other.buf_pos_),
    // from(other.from_),
    // to(other.to_),
    // discret(other.discret_){}

    // SublimedDataInfo(SublimedDataInfo&& other) noexcept:
    // grid_data(std::move(other.grid_data)),
    // buf_pos_(std::move(other.buf_pos_)),
    // from(other.from),
    // to(other.to),
    // discret(other.discret){}
    bool operator==(const SublimedDataInfo& other) const noexcept{
        if(this==&other)
            return true;
        return grid_data_==other.grid_data_ && buf_pos_==other.buf_pos_ && from_==other.from_ && to_ == other.to_ && discret_==other.discret_;
    }
    bool operator!=(const SublimedDataInfo& other) const noexcept{
        return !(*this==other);
    }
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SublimedDataInfo>{
        using type = SublimedDataInfo;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.grid_data_,msg.buf_pos_,msg.from_,msg.to_,msg.discret_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedDataInfo>{
        using type = SublimedDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.grid_data_,msg.buf_pos_,msg.from_,msg.to_,msg.discret_);
        }
    };

    template<>
    struct Serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.grid_data_,msg.buf_pos_,msg.from_,msg.to_,msg.discret_);
        }
    };

    template<>
    struct Min_serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::grid_data_),decltype(type::buf_pos_),decltype(type::from_),decltype(type::to_),decltype(type::discret_)>();
        }();
    };

    template<>
    struct Max_serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::grid_data_),decltype(type::buf_pos_),decltype(type::from_),decltype(type::to_),decltype(type::discret_)>();
        }();
    };
}

// static_assert(serialization::min_serial_size<SublimedDataInfo>()==0);
// static_assert(serialization::max_serial_size<SublimedDataInfo>()==0);