#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <vector>
#include <ranges>
#include <functional>
#include "aux_code/byte_read.h"
#include "sections/def.h"
#include <span>
#include <stdexcept>
struct IndicatorSection{
    unsigned char* buf_;

    IndicatorSection(unsigned char* buffer):buf_(buffer){}

    unsigned char grib_version() const noexcept{
        return buf_[8];
    }
    unsigned long message_length() const noexcept{
        read_bytes<3>(buf_[5],buf_[6],buf_[7]);
    }
};
#else
#include <stdint.h>
typedef struct{
    unsigned char identifier[4];
    unsigned msg_length;
    unsigned char grib_edition;
}IndicatorSection;

extern void read_IS(IndicatorSection* section);
#endif