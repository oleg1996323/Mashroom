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
unsigned long get_BMS_length(const BitMapSection& data);
RepresentationType get_representation_type(const BitMapSection& data);
GridInfo define_grid(const BitMapSection& data,RepresentationType rep_t);
unsigned long get_number_vertical_coord_values(const BitMapSection& data);
unsigned long get_PV(const BitMapSection& data);
unsigned long get_PL(const BitMapSection& data);

#else
#include <stdint.h>

extern void define_GDS()
#endif