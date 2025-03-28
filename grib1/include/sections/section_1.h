#pragma once
#include "sections/def.h"
#include "aux_code/byte_read.h"
#include "code_tables/table_0.h"
#include "code_tables/table_1.h"
#include "code_tables/table_2.h"
#include "code_tables/table_3.h"
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>

STRUCT_BEG(ProductDefinitionSection){
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
}
STRUCT_END(ProductDefinitionSection)

bool check_and_get_section_1(ProductDefinitionSection& section_1,char* buffer,size_t file_size){
	if(buffer == nullptr)
		return false;
	if(file_size<sec_0_sz+sec_1_min_sz)
		return false;
	unsigned length = read_bytes<3>(buffer[0],buffer[1],buffer[2]);
	if(length<sec_1_min_sz)
		return false;
	unsigned char tab_version = buffer[3];
	section_1.centre = (Organization)buffer[4];
	section_1.generatingProcessIdentifier = buffer[5];
	section_1.gridDefinition = buffer[6];
	section_1.section1Flags = (Section2_3_flag)buffer[7];
	section_1.IndicatorOfParameter = (ParameterGrib1)buffer[8];
	section_1.IndicatorOfTypeOfLevel = (LevelsTags)buffer[9];
	section_1.level_data = buffer[10]+buffer[11]<<8;
	section_1.yearOfCentury = buffer[12];
	section_1.month = buffer[13];
	section_1.day = buffer[14];
	section_1.hour = buffer[15];
	section_1.minute = buffer[16];
	section_1.unitOfTimeRange = (UnitTime)buffer[17];
	section_1.P1 = buffer[18];
	section_1.P2 = buffer[19];
	section_1.timeRangeIndicator = (TimeRange)buffer[20];
	section_1.numberIncludedInAverage = buffer[22]<<8+buffer[21];
	section_1.numberMissingFromAveragesOrAccumulations = buffer[23];
	section_1.centuryOfReferenceTimeOfData = buffer[24];
	section_1.subCentre = buffer[25];
	section_1.decimalScaleFactor = buffer[26]+buffer[27]<<8;
}

/* version 3.4 of grib headers  w. ebisuzaki */
/* this version is incomplete */
/* add center DWD    Helmut P. Frank */
/* 10/02 add center CPTEC */
/* 29/04/2005 add center CHM   Luiz Claudio M. Fonseca*/
/* 11/2008 add center LAMI   Davide Sacchetti */

#define PDS_Year(pds)		(pds[12])
#define PDS_Month(pds)		(pds[13])
#define PDS_Day(pds)		(pds[14])
#define PDS_Hour(pds)		(pds[15])
#define PDS_Minute(pds)		(pds[16])
#define PDS_ForecastTimeUnit(pds)	(pds[17])

/* old #define PDS_Year4(pds)   (pds[12] + 100*(pds[24] - (pds[12] != 0))) */
#define PDS_Year4(pds)          (pds[12] + 100*(pds[24] - 1))


#define __LEN24(pds)    ((pds) == NULL ? 0 : (int) ((pds[0]<<16)+(pds[1]<<8)+pds[2]))

#define PDS_Len1(pds)		(pds[0])
#define PDS_Len2(pds)		(pds[1])
#define PDS_Len3(pds)		(pds[2])
#define PDS_LEN(pds)		((int) ((pds[0]<<16)+(pds[1]<<8)+pds[2]))
#define PDS_Vsn(pds)		(pds[3])
#define PDS_Center(pds)		(pds[4])
#define PDS_Model(pds)		(pds[5])
#define PDS_Grid(pds)		(pds[6])
#define PDS_HAS_GDS(pds)	((pds[7] & 128) != 0)
#define PDS_HAS_BMS(pds)	((pds[7] & 64) != 0)
#define PDS_PARAM(pds)		(pds[8])
#define PDS_L_TYPE(pds)		(pds[9])
#define PDS_LEVEL1(pds)		(pds[10])
#define PDS_LEVEL2(pds)		(pds[11])

#define PDS_KPDS5(pds)		(pds[8])
#define PDS_KPDS6(pds)		(pds[9])
#define PDS_KPDS7(pds)		((int) ((pds[10]<<8) + pds[11]))

/* this requires a 32-bit default integer machine */
#define PDS_Field(pds)		((pds[8]<<24)+(pds[9]<<16)+(pds[10]<<8)+pds[11])

#define PDS_P1(pds)		(pds[18])
#define PDS_P2(pds)		(pds[19])
#define PDS_TimeRange(pds)	(pds[20])
#define PDS_NumAve(pds)		((int) ((pds[21]<<8)+pds[22]))
#define PDS_NumMissing(pds)	(pds[23])
#define PDS_Century(pds)	(pds[24])
#define PDS_Subcenter(pds)	(pds[25])
#define PDS_DecimalScale(pds)	INT2(pds[26],pds[27])


/* ECMWF Extensions */

#define PDS_EcLocalId(pds)	(PDS_LEN(pds) >= 41 ? (pds[40]) : 0)
#define PDS_EcClass(pds)	(PDS_LEN(pds) >= 42 ? (pds[41]) : 0)
#define PDS_EcType(pds)		(PDS_LEN(pds) >= 43 ? (pds[42]) : 0)
#define PDS_EcStream(pds)	(PDS_LEN(pds) >= 45 ? (INT2(pds[43], pds[44])) : 0)

#define PDS_EcENS(pds)		(PDS_LEN(pds) >= 52 && pds[40] == 1 && \
				pds[43] * 256 + pds[44] == 1035 && pds[50] != 0)
#define PDS_EcFcstNo(pds)	(pds[49])

#define PDS_Ec16Version(pds)	(pds + 45)
#define PDS_Ec16Number(pds)	(PDS_EcLocalId(pds) == 16 ? UINT2(pds[49],pds[50]) : 0)
#define PDS_Ec16SysNum(pds)	(PDS_EcLocalId(pds) == 16 ? UINT2(pds[51],pds[52]) : 0)
#define PDS_Ec16MethodNum(pds)	(PDS_EcLocalId(pds) == 16 ? UINT2(pds[53],pds[54]) : 0)
#define PDS_Ec16VerfMon(pds)	(PDS_EcLocalId(pds) == 16 ? UINT4(pds[55],pds[56],pds[57],pds[58]) : 0)
#define PDS_Ec16AvePeriod(pds)	(PDS_EcLocalId(pds) == 16 ? pds[59] : 0)
#define PDS_Ec16FcstMon(pds)	(PDS_EcLocalId(pds) == 16 ? UINT2(pds[60],pds[61]) : 0)

/* NCEP Extensions */

#define PDS_NcepENS(pds)	(PDS_LEN(pds) >= 44 && pds[25] == 2 && pds[40] == 1)
#define PDS_NcepFcstType(pds)	(pds[41])
#define PDS_NcepFcstNo(pds)	(pds[42])
#define PDS_NcepFcstProd(pds)	(pds[43])

#define MINUTE  0
#define HOUR    1
#define DAY     2
#define MONTH   3
#define YEAR    4
#define DECADE  5
#define NORMAL  6
#define CENTURY 7
#define HOURS3  10
#define HOURS6  11
#define HOURS12  12
#define MINUTES15 13
#define MINUTES30 14
#define SECOND  254


#define  FEB29   (31+29)
static int monthjday[13] = {
	0,31,59,90,120,151,181,212,243,273,304,334,365};

int verf_time(unsigned char *pds, int *year, int *month, int *day, int *hour);
int add_time(int *year, int *month, int *day, int *hour, int dtime, int unit);

/*
 * PDStimes.c   v1.2 wesley ebisuzaki
 *
 * prints something readable for time code in grib file
 *
 * not all cases decoded
 * for NCEP/NCAR Reanalysis
 *
 * v1.2.1 1/99 fixed forecast time unit table
 * v1.2.2 10/01 add time_range = 11 (at DWD)  Helmut P. Frank
 * v1.2.3 10/05 add time units 13 = 15 min, 14 = 30 min, and
 *              time range 13 = nudging analysis, 14 = relabeled forecast
 *              (at DWD), Helmut P. Frank
 */

 static const char *units[] = {
	"min", "hr", "d", "mon", "yr",
	"decade", "normal", "century", "??", "??", " x3 hours", " x6 hours",
        " x12 hours",
        "x15 min", "x30 min", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
        "??", " sec"}; 

void PDStimes(int time_range, int p1, int p2, int time_unit);