#pragma once
#include "code_tables/table_6.h"
#include "sections/section_3.h"
#include "sections/def.h"
#include "sections/grid/def.h"
#include "sections/grid/grid.h"

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

STRUCT_BEG(BitMapSection)
{
	unsigned char* buf_;
	#ifdef __cplusplus
	BitMapSection(unsigned char* buffer):buf_(buffer){}
	#endif
}
STRUCT_END(BitMapSection)

#ifndef __cplusplus
extern bool define_GDS(BitMapSection* gds,char* buffer,size_t file_size);
#endif

#ifdef __cplusplus
#include <fstream>
#include <vector>
unsigned long get_BMS_length(const BitMapSection& data){
	if(!data.buf_)
		return 0;
	else return read_bytes<3>(data.buf_[0],data.buf_[1],data.buf_[2]);
}
RepresentationType get_representation_type(const BitMapSection& data){
	if(!data.buf_)
		return (RepresentationType)-1;
}
GridInfo define_grid(const BitMapSection& data,RepresentationType rep_t){
    if(!rep_t<256 || !is_representation[rep_t])
        throw std::invalid_argument("Invalid representation type.");
    else
        return GridInfo(GridDataType(data.buf_,rep_t),rep_t);
}
unsigned long get_number_vertical_coord_values(const BitMapSection& data){
	return GDS_NV(data.buf_);
}
unsigned long get_PV(const BitMapSection& data){
	
}
unsigned long get_PL(const BitMapSection& data){
	
}

#else
#include <stdint.h>

extern void define_GDS()
#endif