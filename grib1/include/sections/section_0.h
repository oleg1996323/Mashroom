#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <vector>
#include <ranges>
#include "bit/byte_read.h"
#include "sections/def.h"
struct IndicatorSection{
    unsigned msg_length;
    unsigned char grib_edition; 
};
bool check_and_get_section_0(IndicatorSection& section_0,char* buffer,size_t file_size){
    if(buffer == nullptr)
        return false;
    if(file_size<sec_0_sz)
        return false;
    if(!(buffer[0]=='G' && buffer[1]=='R' && buffer[2]=='I' && buffer[3]=='B'))
        return false;
    section_0.msg_length = read_bytes<3>(buffer[5],buffer[6],buffer[7]);
    section_0.grib_edition = buffer[8];
}
#else
#include <stdint.h>
typedef struct{
    unsigned char identifier[4];
    unsigned msg_length;
    unsigned char grib_edition;
}IndicatorSection;

extern void read_IS(IndicatorSection* section);
#endif