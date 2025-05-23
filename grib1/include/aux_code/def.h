#pragma once
#include <cstdint>

constexpr bool is_little_endian() {
    union {
        uint16_t value;
        uint8_t bytes[2];
    } test = {0x0102}; // 0x01 (старший байт), 0x02 (младший байт)
    return test.bytes[0] == 0x02; // Если первый байт = 0x02, то LSB (little-endian)
}
#define _IS_LSB_ is_little_endian();
#define LSB _IS_LSB_

#ifndef __cplusplus
#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  ((a) < (b) ? (b) : (a))
#endif
#endif

#ifdef __cplusplus
#define STRUCT_BEG(x) struct x
#define STRUCT_END(x) ;
#define UNION_BEG(x) union x
#define UNION_END(x) ;
#define TEMPLATE_STRUCT_BEG(str_name,T) template<typename T> \
                                        struct str_name
#define TEMPLATE_STRUCT_END(x) ;
#define SPECIFIED_TEMPLATE_STRUCT_BEG(str_name,T) template<> \
                                                struct str_name<T>
#define SPECIFIED_TEMPLATE_STRUCT_END(x) ;     
#define DEF_STRUCT_VAL(val) = val;                                        
#else 
#define STRUCT_BEG(x) typedef struct
#define STRUCT_END(x) x;
#define UNION_BEG(x) typedef union
#define UNION_END(x) x;
#define DEF_STRUCT_VAL(val) ;
#endif