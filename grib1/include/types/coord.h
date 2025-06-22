#pragma once
#include "functional/def.h"
#include <functional/serialization.h>
#include "functional/float_conv.h"
typedef float Lat;
typedef float Lon;

struct Coord
{
    Lat lat_ = -999;
    Lon lon_ = -999;

    bool is_correct_pos() const{
        if(lon_>=0 && lon_<=180 && lat_<=90 && lat_>=-90)
            return true;
        else return false;
    }
};

struct RawCoord
{
    uint32_t lat_ = -999;
    uint32_t lon_ = -999;
};

extern bool is_correct_pos(const Coord* pos);
bool is_correct_pos(const Coord& pos);

namespace serialization{

template<bool NETWORK_ORDER>
struct Serialize<NETWORK_ORDER,Coord>{
    SerializationEC operator()(const Coord& pos, std::vector<char>& buf) noexcept{
        return serialize<NETWORK_ORDER>(pos,buf,pos.lat_,pos.lon_);
    }
};

template<bool NETWORK_ORDER>
struct Deserialize<NETWORK_ORDER,Coord>{
    SerializationEC operator()(Coord& pos,std::span<const char> buf) noexcept{
        return deserialize<NETWORK_ORDER>(pos,buf,pos.lat_,pos.lon_);
    }
};
template<>
struct Serial_size<Coord>{
    static constexpr size_t operator()(const Coord& val) noexcept{
        return serial_size(val.lat_,val.lon_);
    }
};
template<>
struct Min_serial_size<Coord>{
    static constexpr size_t operator()(const Coord& val) noexcept{
        return sizeof(val.lat_)+sizeof(val.lon_);
    }
};
template<>
struct Max_serial_size<Coord>{
    static constexpr size_t operator()(const Coord& val) noexcept{
        return sizeof(val.lat_)+sizeof(val.lon_);
    }
};
}