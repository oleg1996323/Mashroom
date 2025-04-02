#pragma once
#include "code_tables/table_6.h"
#include "sections/section_3.h"
#include "sections/def.h"
#include "sections/grid/def.h"
#include "sections/grid/grid.h"

#ifdef __cplusplus
#include <cstdint>
#include <fstream>
#include <vector>
#else
#include <stdint.h>
#endif

STRUCT_BEG(BitMapSection)
{
	unsigned char* buf_;
	#ifdef __cplusplus
	BitMapSection(unsigned char* buffer):buf_(buffer){}
	unsigned long section_length();
	unsigned long unused_bits();
	unsigned short table_ref();
	unsigned char* data();
	#endif
}
STRUCT_END(BitMapSection)

#ifndef __cplusplus
#include <stdint.h>
extern bool define_GDS(BitMapSection* gds,char* buffer,size_t file_size);
#endif