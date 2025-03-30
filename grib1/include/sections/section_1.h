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
	unsigned char* buffer;
	unsigned section1Length;
	unsigned table2Version;
	Organization centre;
	uint8_t generatingProcessIdentifier;
	uint8_t gridDefinition;
	Section2_3_flag section1Flags;
	ParameterGrib1 IndicatorOfParameter;
	LevelsTags IndicatorOfTypeOfLevel;
	uint16_t level_data;
	uint8_t yearOfCentury;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	UnitTime unitOfTimeRange;
	uint8_t P1;
	uint8_t P2;
	TimeRange timeRangeIndicator;
	uint16_t numberIncludedInAverage;
	uint8_t numberMissingFromAveragesOrAccumulations;
	uint8_t centuryOfReferenceTimeOfData;
	uint8_t subCentre;
	int16_t decimalScaleFactor;
	char reserved[12];
	char* additional;
	bool defined DEF_STRUCT_VAL(false)

	#ifdef __cplusplus
	ProductDefinitionSection(unsigned char* buffer){
		// if(buffer.size()<sec_0_min_sz+sec_1_min_sz){
		// 	defined = false;
		// 	return;
		// }
		// unsigned length = read_bytes<3>(buffer[0],buffer[1],buffer[2]);
		// if(length<sec_1_min_sz){
		// 	defined = false;
		// 	return;
		// }
		unsigned char tab_version = buffer[3];
		centre = (Organization)buffer[4];
		generatingProcessIdentifier = buffer[5];
		gridDefinition = buffer[6];
		section1Flags = (Section2_3_flag)buffer[7];
		IndicatorOfParameter = (ParameterGrib1)buffer[8];
		IndicatorOfTypeOfLevel = (LevelsTags)buffer[9];
		level_data = buffer[10]+buffer[11]<<8;
		yearOfCentury = buffer[12];
		month = buffer[13];
		day = buffer[14];
		hour = buffer[15];
		minute = buffer[16];
		unitOfTimeRange = (UnitTime)buffer[17];
		P1 = buffer[18];
		P2 = buffer[19];
		timeRangeIndicator = (TimeRange)buffer[20];
		numberIncludedInAverage = buffer[22]<<8+buffer[21];
		numberMissingFromAveragesOrAccumulations = buffer[23];
		centuryOfReferenceTimeOfData = buffer[24];
		subCentre = buffer[25];
		decimalScaleFactor = buffer[26]+buffer[27]<<8;
		defined = true;
	}
	unsigned section_length(){
		return read_bytes<3>(buffer[0],buffer[1],buffer[2]);
	}
	unsigned char table_version(){
		return PDS_Vsn(buffer);
	}
	Organization center(){
		return (Organization)PDS_Center(buffer);
	}
	unsigned char generatingProcessIdentifier(){
		return PDS_Model(buffer);
	}
	unsigned char grid_definition(){
		return PDS_Grid(buffer);
	}
	Section2_3_flag section1Flags(){
		return {PDS_HAS_GDS(buffer),PDS_HAS_BMS(buffer)};
	}
	unsigned char IndicatorOfParameter(){
		return PDS_PARAM(buffer);
	}
	LevelsTags level(){
		return (LevelsTags)PDS_L_TYPE(buffer);
	}
	unsigned char level1_data(){
		return PDS_LEVEL1(buffer);
	}
	unsigned char level2_data(){
		return PDS_LEVEL2(buffer);
	}
	unsigned char subcenter(){
		return PDS_Subcenter(buffer);
	}
	unsigned char month(){
		return PDS_Month(buffer);
	}
	unsigned char day(){
		return PDS_Day(buffer);
	}
	unsigned char hour(){
		return PDS_Hour(buffer);
	}
	unsigned char century(){
		return PDS_Century(buffer);
	}
	unsigned numberMissingFromAveragesOrAccumulations(){
		return PDS_NumMissing(buffer);
	}
	unsigned char numberIncludedInAverage(){
		return PDS_NumAve(buffer);
	}
	#endif
}
STRUCT_END(ProductDefinitionSection)