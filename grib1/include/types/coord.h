#pragma once
#include "functional/def.h"
#include <functional/serialization.h>
#include "functional/float_conv.h"
typedef float Lat;
typedef float Lon;

struct Coord
{
    Lat lat_ DEF_STRUCT_VAL(-999);
    Lon lon_ DEF_STRUCT_VAL(-999);

    bool is_correct_pos() const{
        if(lon_>=0 && lon_<=180 && lat_<=90 && lat_>=-90)
            return true;
        else return false;
    }
};

STRUCT_BEG(RawCoord)
{
    uint32_t lat_ DEF_STRUCT_VAL(-999);
    uint32_t lon_ DEF_STRUCT_VAL(-999);
}
STRUCT_END(RawCoord)

#ifndef __cplusplus
#define Coord(...) ((Coord) { .lat_ = -999, .lon_ = -999},##__VA_ARGS__)
#endif

extern bool is_correct_pos(const Coord* pos);
bool is_correct_pos(const Coord& pos);



namespace serialization{

template<>
size_t serial_size(const Coord& val) noexcept{
    return serial_size(val.lat_,val.lon_);
}
template<>
constexpr size_t min_serial_size(const Coord& val) noexcept{
    return serial_size(val.lat_,val.lon_);
}
template<>
constexpr size_t max_serial_size(const Coord& val) noexcept{
    return serial_size(val.lat_,val.lon_);
}

template<>
SerializationEC serialize<true,Coord>(const Coord& pos, std::vector<char>& buf) noexcept{
    return serialize<true>(pos,buf,pos.lat_,pos.lon_);
}
template<>
SerializationEC deserialize<true,Coord>(Coord& pos,std::span<const char> buf) noexcept{
    return deserialize<true>(pos,buf,pos.lat_,pos.lon_);
}
}