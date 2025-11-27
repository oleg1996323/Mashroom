#pragma once
#include <fstream>
#include "types/time_interval.h"
#include "serialization.h"
#include "sections/grid/grid.h"
#include "byte_read.h"
#include "types/time_interval.h"
#include "data/def.h"

template<Data_t TYPE, Data_f FORMAT>
struct SublimedDataInfoStruct;

template<>
struct SublimedDataInfoStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    std::optional<GridInfo> grid_data_;
    std::vector<ptrdiff_t> buf_pos_;
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    TimeSequence sequence_time_ = TimeSequence(utc_tp::max(),utc_tp::min(),std::chrono::system_clock::duration(0));
    bool operator==(const SublimedDataInfoStruct& other) const noexcept{
        if(this==&other)
            return true;
        return grid_data_==other.grid_data_ && buf_pos_==other.buf_pos_ && sequence_time_==other.sequence_time_;
    }
    bool operator!=(const SublimedDataInfoStruct& other) const noexcept{
        return !(*this==other);
    }
};

using GribSublimedDataInfoStruct = SublimedDataInfoStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,GribSublimedDataInfoStruct>{
        using type = GribSublimedDataInfoStruct;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.grid_data_,msg.buf_pos_,msg.sequence_time_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,GribSublimedDataInfoStruct>{
        using type = GribSublimedDataInfoStruct;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.grid_data_,msg.buf_pos_,msg.sequence_time_);
        }
    };

    template<>
    struct Serial_size<GribSublimedDataInfoStruct>{
        using type = GribSublimedDataInfoStruct;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.grid_data_,msg.buf_pos_,msg.sequence_time_);
        }
    };

    template<>
    struct Min_serial_size<GribSublimedDataInfoStruct>{
        using type = GribSublimedDataInfoStruct;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::grid_data_),decltype(type::buf_pos_),decltype(type::sequence_time_)>();
        }();
    };

    template<>
    struct Max_serial_size<GribSublimedDataInfoStruct>{
        using type = GribSublimedDataInfoStruct;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::grid_data_),decltype(type::buf_pos_),decltype(type::sequence_time_)>();
        }();
    };
}

// static_assert(serialization::min_serial_size<SublimedDataInfo>()==0);
// static_assert(serialization::max_serial_size<SublimedDataInfo>()==0);