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

struct BitMapSection
{
	unsigned char* buf_;
	#ifdef __cplusplus
	BitMapSection(unsigned char* buffer):buf_(buffer){}
	unsigned long section_length();
	unsigned long unused_bits();
	unsigned short table_ref();
	unsigned char* data();
	#endif
};