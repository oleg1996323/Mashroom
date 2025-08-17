#pragma once
#include "def.h"
#include "byte_read.h"
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
//#define PDS_Field(pds)		((pds[8]<<24)+(pds[9]<<16)+(pds[10]<<8)+pds[11])
inline auto PDS_Field(unsigned char* ptr){
	return read_bytes<4,false>(&ptr[8]);
}
//#define PDS_P1(pds)		(pds[18])
inline auto PDS_P1(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[18]);
}
//#define PDS_P2(pds)		(pds[19])
inline auto PDS_P2(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[19]);
}
//#define PDS_TimeRange(pds)	(pds[20])
inline auto PDS_TimeRange(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[20]);
}
//#define PDS_NumAve(pds)		((int) ((pds[21]<<8)+pds[22]))
inline auto PDS_NumAve(unsigned char* ptr){
	return read_bytes<2,true>(&ptr[21]);
}
//#define PDS_NumMissing(pds)	(pds[23])
inline auto PDS_NumMissing(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[23]);
}
//#define PDS_Century(pds)	(pds[24])
inline auto PDS_Century(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[24]);
}
//#define PDS_Subcenter(pds)	(pds[25])
inline auto PDS_Subcenter(unsigned char* ptr){
	return read_bytes<1,false>(&ptr[25]);
}
//#define PDS_DecimalScale(pds)	INT2(pds[26],pds[27])
inline auto PDS_DecimalScale(unsigned char* ptr){
	return read_bytes<2,true>(&ptr[26]);
}


/* ECMWF Extensions */

#define PDS_LocalId(pds)	(PDS_LEN(pds) >= 41 ? (pds[40]) : 0)
#define PDS_EcClass(pds)	(PDS_LEN(pds) >= 42 ? (pds[41]) : 0)
#define PDS_EcType(pds)		(PDS_LEN(pds) >= 43 ? (pds[42]) : 0)
#define PDS_EcStream(pds)	(PDS_LEN(pds) >= 45 ? (INT2(pds[43], pds[44])) : 0)

#define PDS_EcENS(pds)		(PDS_LEN(pds) >= 52 && pds[40] == 1 && \
				pds[43] * 256 + pds[44] == 1035 && pds[50] != 0)
#define PDS_EcFcstNo(pds)	(pds[49])

#define PDS_Ec16Version(pds)	(pds + 45)
#define PDS_Ec16Number(pds)	(PDS_LocalId(pds) == 16 ? UINT2(pds[49],pds[50]) : 0)
#define PDS_Ec16SysNum(pds)	(PDS_LocalId(pds) == 16 ? UINT2(pds[51],pds[52]) : 0)
#define PDS_Ec16MethodNum(pds)	(PDS_LocalId(pds) == 16 ? UINT2(pds[53],pds[54]) : 0)
#define PDS_Ec16VerfMon(pds)	(PDS_LocalId(pds) == 16 ? UINT4(pds[55],pds[56],pds[57],pds[58]) : 0)
#define PDS_Ec16AvePeriod(pds)	(PDS_LocalId(pds) == 16 ? pds[59] : 0)
#define PDS_Ec16FcstMon(pds)	(PDS_LocalId(pds) == 16 ? UINT2(pds[60],pds[61]) : 0)

/* NCEP Extensions */

#define PDS_NcepENS(pds)	(PDS_LEN(pds) >= 44 && pds[25] == 2 && pds[40] == 1)
#define PDS_NcepFcstType(pds)	(pds[41])
#define PDS_NcepFcstNo(pds)	(pds[42])
#define PDS_NcepFcstProd(pds)	(pds[43])

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

void PDStimes(int time_range, int p1, int p2, int time_unit);