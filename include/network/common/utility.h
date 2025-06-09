#pragma once
#include <cinttypes>
#include <ctype.h>
#include <netinet/in.h>

namespace network::utility{
constexpr bool is_little_endian() {
    union {
        uint16_t value;
        uint8_t bytes[2];
    } test = {0x0102}; // 0x01 (старший байт), 0x02 (младший байт)
    return test.bytes[0] == 0x02; // Если первый байт = 0x02, то LSB (little-endian)
}

uint64_t htonll(uint64_t value){
    if (is_little_endian())
    {
        return __builtin_bswap64(value);
    } 
    else
        return value;
}
uint64_t ntohll(uint64_t value){
    return htonll(value);
}
float htonf(float value){
    if (is_little_endian())
    {
        return std::bit_cast<float,uint32_t>(__builtin_bswap32(std::bit_cast<uint32_t,float>(value)));
    } 
    else
        return value;
}
float ntohf(float value){
    return htonf(value);
}
double htond(double value){
    if (is_little_endian())
    {
        return std::bit_cast<double,uint64_t>(__builtin_bswap32(std::bit_cast<uint64_t,double>(value)));
    } 
    else
        return value;
}
double ntohd(double value){
    return htond(value);
}
}