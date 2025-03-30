#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <vector>
#include <ranges>
#include "aux_code/byte_read.h"
#include "sections/def.h"
#include <span>
struct IndicatorSection{
    unsigned msg_length;
    unsigned char grib_edition;
    bool defined DEF_STRUCT_VAL(false);

    IndicatorSection(unsigned char* buffer){
        if(buffer.size()<sec_0_min_sz){
            defined = false;
            return;
        }
        if(!((char*)buffer.first(4).data()=="GRIB")){
            defined = false;
            return;
        }
        msg_length = read_bytes<3>(buffer[5],buffer[6],buffer[7]);
        grib_edition = buffer[8];
        defined = true;
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