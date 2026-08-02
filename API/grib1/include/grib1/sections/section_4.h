#pragma once
#include "grib1/code_tables.h"
#include "section_3.h"
#include "def.h"
#include "grib1/sections/grid/def.h"
#include "grib1/sections/grid/grid.h"
#include "binary/def.h"
#include "OsterLib/types/coord.h"

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
		Flag result;
		result.spherical_harm_coefs=static_cast<bool>(BDS_Harmonic(buf_));
		result.complex_pack=static_cast<bool>(BDS_ComplexPacking(buf_));
		result.int_values=static_cast<bool>(BDS_OriginalInt(buf_));
		result.oct_14_flag_bits=static_cast<bool>(BDS_MoreFlags(buf_));
		result.matrix = result.oct_14_flag_bits?static_cast<bool>(BDS_MatrixDatum(buf_)):false;
		result.second_bmp=result.oct_14_flag_bits?static_cast<bool>(BDS_SecondairyBMP(buf_)):false;
		result.second_order_diff_widths=result.oct_14_flag_bits?static_cast<bool>(BDS_SecondOrdValsDiffWidth(buf_)):false;
		return result;
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