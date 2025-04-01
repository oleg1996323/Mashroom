#pragma once
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include "sections/def.h"
#include "aux_code/byte_read.h"
#include "code_tables/table_0.h"
#include "code_tables/table_1.h"
#include "code_tables/table_2.h"
#include "code_tables/table_3.h"
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include "sections/product/def.h"

#ifdef __cplusplus
#include <span>
#endif


/* #define LEN_HEADER_PDS (28+42+100) */
#define LEN_HEADER_PDS (28+8)

STRUCT_BEG(ProductDefinitionSection)
{
	unsigned char* buffer_;
	unsigned section1Length_;
	unsigned table2Version_;
	Organization centre_;
	uint8_t generatingProcessIdentifier_;
	uint8_t gridDefinition_;
	Section2_3_flag section1Flags_;
	ParameterGrib1 IndicatorOfParameter_;
	LevelsTags IndicatorOfTypeOfLevel_;
	uint16_t level_data_;
	uint8_t yearOfCentury_;
	uint8_t month_;
	uint8_t day_;
	uint8_t hour_;
	uint8_t minute_;
	TimeFrame unitOfTimeRange_;
	uint8_t P1_;
	uint8_t P2_;
	TimeRange timeRangeIndicator_;
	uint16_t numberIncludedInAverage_;
	uint8_t numberMissingFromAveragesOrAccumulations_;
	uint8_t centuryOfReferenceTimeOfData_;
	uint8_t subCentre_;
	int16_t decimalScaleFactor_;
	char reserved[12];
	char* additional_;
	bool defined DEF_STRUCT_VAL(false)

	#ifdef __cplusplus
	ProductDefinitionSection(unsigned char* buffer):buffer_(buffer){
		// if(buffer.size()<sec_0_min_sz+sec_1_min_sz){
		// 	defined = false;
		// 	return;
		// }
		// unsigned length = read_bytes<3>(buffer_[0],buffer_[1],buffer_[2]);
		// if(length<sec_1_min_sz){
		// 	defined = false;
		// 	return;
		// }
		unsigned char tab_version = buffer_[3];
	}
	unsigned section_length(){
		return read_bytes<3>(buffer_[0],buffer_[1],buffer_[2]);
	}
	unsigned char table_version(){
		return PDS_Vsn(buffer_);
	}
	Organization center(){
		return (Organization)PDS_Center(buffer_);
	}
	unsigned char generatingProcessIdentifier(){
		return PDS_Model(buffer_);
	}
	unsigned char grid_definition(){
		return PDS_Grid(buffer_);
	}
	Section2_3_flag section1Flags(){
		return {PDS_HAS_GDS(buffer_),PDS_HAS_BMS(buffer_)};
	}
	unsigned char IndicatorOfParameter(){
		return PDS_PARAM(buffer_);
	}
	LevelsTags level(){
		return (LevelsTags)PDS_L_TYPE(buffer_);
	}
	unsigned char level1_data(){
		return PDS_LEVEL1(buffer_);
	}
	unsigned char level2_data(){
		return PDS_LEVEL2(buffer_);
	}
	unsigned char subcenter(){
		return PDS_Subcenter(buffer_);
	}
	unsigned char month(){
		return PDS_Month(buffer_);
	}
	unsigned char day(){
		return PDS_Day(buffer_);
	}
	unsigned char hour(){
		return PDS_Hour(buffer_);
	}
	unsigned char century(){
		return PDS_Century(buffer_);
	}
	unsigned numberMissingFromAveragesOrAccumulations(){
		return PDS_NumMissing(buffer_);
	}
	unsigned char numberIncludedInAverage(){
		return PDS_NumAve(buffer_);
	}
	#endif
}
STRUCT_END(ProductDefinitionSection)