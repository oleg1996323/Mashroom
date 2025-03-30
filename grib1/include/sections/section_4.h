#pragma once
#include "code_tables/table_6.h"
#include "sections/section_3.h"
#include "sections/def.h"
#include "sections/grid/def.h"
#include "sections/grid/grid.h"
#include "sections/binary/def.h"
#include "code_tables/table_11.h"

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

STRUCT_BEG(BinaryDataSection)
{
	unsigned char* buf_;
	#ifdef __cplusplus
	BinaryDataSection(unsigned char* buffer):buf_(buffer){}

	unsigned long get_BDS_length(){
		if(!buf_)
			return 0;
		else return BDS_LEN(buf_);
	}
	Flag get_data_flag(){
		return Flag{BDS_Harmonic(buf_),BDS_ComplexPacking(buf_),
				BDS_OriginalInt(buf_),BDS_MoreFlags(buf_),
				BDS_MoreFlags(buf_)?BDS_MatrixDatum(buf_):-1,
				BDS_MoreFlags(buf_)?BDS_SecondairyBMP(buf_):-1
				,BDS_MoreFlags(buf_)?BDS_SecondOrdValsDiffWidth(buf_):-1};
	}
	int16_t scale_factor(){
		return BDS_BinScale(buf_);
	}
	float ref_value(){
		return BDS_RefValue(buf_);
	}
	uint8_t bit_per_value(){
		return BDS_NumBits(buf_);
	}
	#endif
}
STRUCT_END(BinaryDataSection)

#ifndef __cplusplus
extern bool define_GDS(BinaryDataSection* gds,char* buffer,size_t file_size);
#endif

#ifdef __cplusplus



#else
#include <stdint.h>

extern void define_GDS()
#endif