#pragma once
#include "functional/def.h"
#include <functional/serialization.h>
#include "functional/float_conv.h"
typedef float Lat;
typedef float Lon;

STRUCT_BEG(Coord)
{
    Lat lat_ DEF_STRUCT_VAL(-999);
    Lon lon_ DEF_STRUCT_VAL(-999);

    bool is_correct_pos() const{
        if(lon_>=0 && lon_<=180 && lat_<=90 && lat_>=-90)
            return true;
        else return false;
    }
}
STRUCT_END(Coord)

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
constexpr size_t serial_size(const Coord& val) noexcept{
    return sizeof(Coord::lat_)+sizeof(Coord::lon_);
}
template<>
constexpr size_t min_serial_size(const Coord& val) noexcept{
    return serial_size(val);
}
template<>
constexpr size_t max_serial_size(const Coord& val) noexcept{
    return serial_size(val);
}

template<bool NETWORK_ORDER = true>
void serialize(Coord pos, std::vector<char>& buf) noexcept{
    if constexpr (NETWORK_ORDER && is_little_endian()){
        auto int_val = to_integer(pos.lat_);
        int_val = std::byteswap(int_val);
        pos.lat_ = to_float(int_val);
        int_val = to_integer(pos.lon_);
        int_val = std::byteswap(int_val);
        pos.lon_ = to_float(int_val);
    }
    const auto* begin_lat = reinterpret_cast<const char*>(&pos.lat_);
    buf.insert(buf.end(), begin_lat, begin_lat + sizeof(pos.lat_));
    const auto* begin_lon = reinterpret_cast<const char*>(&pos.lat_);
    buf.insert(buf.end(), begin_lon, begin_lon + sizeof(pos.lon_));
}

template<>
auto deserialize<Coord,true>(std::span<const char> buf) noexcept -> std::expected<Coord,SerializationEC>{
    if(buf.size()<serial_size(Coord{}))
        return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
    Coord result;
    if(is_little_endian()){
        {
            auto des_res = deserialize_network<decltype(Coord::lat_)>(buf);
            if(des_res.has_value())
                result.lat_ = des_res.value();
            else return std::unexpected(des_res.error());
        }
        {
            auto des_res = deserialize_network<decltype(Coord::lon_)>(buf);
            if(des_res.has_value())
                result.lon_ = des_res.value();
            else return std::unexpected(des_res.error());
        }
    }
    return result;
}

template<>
auto deserialize<Coord,false>(std::span<const char> buf) noexcept -> std::expected<Coord,SerializationEC>{
    if(buf.size()<serial_size(Coord{}))
        return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
    Coord result;
    {
        auto des_res = deserialize_native<decltype(Coord::lat_)>(buf);
        if(des_res.has_value())
            result.lat_ = des_res.value();
        else return std::unexpected(des_res.error());
    }
    {
        auto des_res = deserialize_native<decltype(Coord::lon_)>(buf);
        if(des_res.has_value())
            result.lon_ = des_res.value();
        else return std::unexpected(des_res.error());
    }
    return result;
}
}