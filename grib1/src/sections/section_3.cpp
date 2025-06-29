#include "sections/section_3.h"
#include "sections/bitmap/def.h"

#ifdef __cplusplus
#include <fstream>
#include <vector>

unsigned long BitMapSection::section_length(){
	return read_bytes<3,false>(&buf_[0]);
}
unsigned long BitMapSection::unused_bits(){
	return BMS_UnusedBits(buf_);
}
unsigned short BitMapSection::table_ref(){
    return BMS_StdMap(buf_);
}
unsigned char* BitMapSection::data(){
	return BMS_bitmap(buf_);
}

#else
#include <stdint.h>

extern void define_GDS(){
    
}
#endif