#pragma once
#include <cstdint>

#define UNDEFINED		9.999e20
static bool is_little_endian() noexcept {
    static const union {
        uint16_t value;
        uint8_t bytes[2];
    } test = {0x0102}; // 0x01 (старший байт), 0x02 (младший байт)
    return test.bytes[0] == 0x02; // Если первый байт = 0x02, то LSB (little-endian)
}

#define LSB _IS_LSB_

#ifndef __cplusplus
#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  ((a) < (b) ? (b) : (a))
#endif
#endif