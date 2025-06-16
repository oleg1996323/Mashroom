#pragma once
#include <cinttypes>
#include <ctype.h>
#include <netinet/in.h>
#include <functional/def.h>
#include <bit>

namespace network::utility{
uint64_t htonll(uint64_t value){
    if (is_little_endian())
        return std::byteswap(value);
    else
        return value;
}
uint64_t ntohll(uint64_t value){
    return htonll(value);
}
float htonf(float value){
    if (is_little_endian())
        return std::bit_cast<float,uint32_t>(std::byteswap(std::bit_cast<uint32_t,float>(value)));
    else
        return value;
}
float ntohf(float value){
    return htonf(value);
}
double htond(double value){
    if (is_little_endian())
        return std::bit_cast<double,uint64_t>(std::byteswap(std::bit_cast<uint64_t,double>(value)));
    else
        return value;
}
double ntohd(double value){
    return htond(value);
}
}