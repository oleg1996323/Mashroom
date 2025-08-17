#pragma once
#include <cinttypes>
#include <type_traits>
#include "byte_order.h"
#include "float_conv.h"

#ifndef INT2
#define INT2(a,b)   ((1-(int) ((unsigned) (a & 0x80) >> 6)) * (int) (((a & 0x7f) << 8) + b))
#endif
#ifndef UINT2
#define UINT2(a,b) ((int) ((a << 8) + (b)))
#endif
#ifndef INT3
#define INT3(a,b,c) ((1-(int) ((unsigned) (a & 0x80) >> 6)) * (int) (((a & 127) << 16)+(b<<8)+c))
#endif
#ifndef UINT3
#define UINT3(a,b,c) ((int) ((a << 16) + (b << 8) + (c)))
#endif
#ifndef INT4
#define INT4(a,b,c,d) ((1-(int) ((unsigned) (a & 0b10000000) >> 6)) * (int) (((a & 0b1111111) << 24)+(b<<16)+(c<<8)+d))
#endif
#ifndef UINT4
#define UINT4(a,b,c,d) ((int) ((a << 24) + (b << 16) + (c << 8) + (d)))
#endif

template<typename... ARGS,bool signed_t=false,bool BE = true>
requires ((std::is_same_v<std::remove_cv_t<ARGS>,char> && ...) || 
        (std::is_same_v<std::remove_cv_t<ARGS>,unsigned char> && ...))
inline auto read_bytes(ARGS&&... args){
    /// @todo verify/test
    using type = decltype(detail::to_integer_type<sizeof...(args),signed_t>());
    type result = 0;
    unsigned shift = 0;
    if(is_little_endian()){
        shift = sizeof...(ARGS)-1;
        result = ((static_cast<type>(args) << (shift-- * 8)) | ...);
    }
    else{
        result = ((static_cast<type>(args) << (shift++ * 8)) | ...);
    }
    return result;
}
#include <cassert>
template<int sz, bool signed_t = false>
requires (sz<=8 && sz>0)
inline auto read_bytes(unsigned char* ptr){
    assert(ptr);
/*     if constexpr (sz==2){
        if constexpr (signed_t == true)
            return INT2(ptr[0],ptr[1]);
        else return UINT2(ptr[0],ptr[1]);
    }
    else if constexpr(sz==3){
        if constexpr (signed_t == true)
            return INT3(ptr[0],ptr[1],ptr[2]);
        else return UINT3(ptr[0],ptr[1],ptr[2]);
    }
    else if constexpr(sz==4){
        if constexpr (signed_t == true)
            return INT4(ptr[0],ptr[1],ptr[2],ptr[3]);
        else return UINT4(ptr[0],ptr[1],ptr[2],ptr[3]);
    }
    else */{
        /// @todo verify/test
        using type = decltype(detail::to_integer_type<sz,signed_t>());
        type result=0;
        if(is_little_endian()){
            for(int8_t i=0;i<sz;++i)
                result |= (static_cast<type>(ptr[i]) << (8 * (sz-1-i)));
        }
        else{
            for(int8_t i=0;i<sz;++i)
                result |= (static_cast<type>(ptr[i]) << (8 * i));
        }
        return result;
    }
}


