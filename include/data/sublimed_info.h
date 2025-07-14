#pragma once
#include <fstream>
#include "types/time_interval.h"
#include "serialization.h"
#include "sections/grid/grid.h"
#include "byte_read.h"

struct SublimedDataInfo
{
    std::optional<GridInfo> grid_data;
    std::vector<ptrdiff_t> buf_pos_;
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    std::chrono::system_clock::time_point from = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point to = std::chrono::system_clock::time_point::min();
    std::chrono::system_clock::duration discret = std::chrono::system_clock::duration(0);
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SublimedDataInfo>{
        using type = SublimedDataInfo;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.grid_data,msg.buf_pos_,msg.from,msg.to,msg.discret);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedDataInfo>{
        using type = SublimedDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.grid_data,msg.buf_pos_,msg.from,msg.to,msg.discret);
        }
    };

    template<>
    struct Serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.grid_data,msg.buf_pos_,msg.from,msg.to,msg.discret);
        }
    };

    template<>
    struct Min_serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.grid_data,msg.buf_pos_,msg.from,msg.to,msg.discret);
        }
    };

    template<>
    struct Max_serial_size<SublimedDataInfo>{
        using type = SublimedDataInfo;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.grid_data,msg.buf_pos_,msg.from,msg.to,msg.discret);
        }
    };
}