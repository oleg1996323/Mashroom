#pragma once
#include "code_tables/table_6.h"
#include "sections/section_3.h"
#include "sections/def.h"
#include "sections/grid/def.h"
#include "sections/grid/grid.h"
#include "sections/binary/def.h"
#include "code_tables/table_11.h"
#include "types/coord.h"

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif
#include <iostream>
struct BinaryDataSection
{
	unsigned char* buf_;
	#ifdef __cplusplus
	BinaryDataSection(unsigned char* buffer):buf_(buffer){}

	unsigned long get_BDS_length(){
		if(buf_){
			auto res = read_bytes<3,false>(buf_);
			assert(res==UINT3(buf_[0],buf_[1],buf_[2]));
			return res;
		}
		else return 0;
	}
	Flag get_data_flag(){
		return Flag{BDS_Harmonic(buf_),BDS_ComplexPacking(buf_),
				BDS_OriginalInt(buf_),BDS_MoreFlags(buf_),
				BDS_MoreFlags(buf_)?BDS_MatrixDatum(buf_):-1,
				BDS_MoreFlags(buf_)?BDS_SecondairyBMP(buf_):-1
				,BDS_MoreFlags(buf_)?BDS_SecondOrdValsDiffWidth(buf_):-1};
	}
	//#define BDS_BinScale(bds) INT2(bds[4],bds[5])
	int16_t scale_factor(){
		return INT2(buf_[4],buf_[5]);
	}
	//#define BDS_RefValue(bds) (ibm2flt(bds+6))
	float ref_value(){
		return BDS_RefValue(buf_);
	}
	uint8_t bit_per_value(){
		return BDS_NumBits(buf_);
	}
	#endif
};