#pragma once
#include "code_tables/table_6.h"
#include "aux_code/byte_read.h"

#define GDS_Len1(gds)		(gds[0])
#define GDS_Len2(gds)		(gds[1])
#define GDS_Len3(gds)		(gds[2])
#define GDS_LEN(gds)		((int) (UINT3(gds[0],gds[1],gds[2])))

#define GDS_NV(gds)		(gds[3])
#define GDS_DataType(gds)	(gds[5])

#define GDS_LatLon(gds)		(gds[5] == RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR)
#define GDS_Mercator(gds)	(gds[5] == RepresentationType::MERCATOR)
#define GDS_Gnomonic(gds)	(gds[5] == RepresentationType::GNOMONIC)
#define GDS_Lambert(gds)	(gds[5] == RepresentationType::LAMBERT)
#define GDS_Gaussian(gds)	(gds[5] == RepresentationType::GAUSSIAN)
#define GDS_Polar(gds)		(gds[5] == RepresentationType::POLAR_STEREOGRAPH_PROJ)
#define GDS_Albers(gds)		(gds[5] == RepresentationType::ALBERS_EQUAL_AREA)
#define GDS_RotLL(gds)		(gds[5] == RepresentationType::ROTATED_LAT_LON)
#define GDS_Harmonic(gds)	(gds[5] == RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS)
#define GDS_Triangular(gds)	(gds[5] == 192)
#define GDS_ssEgrid(gds)	(gds[5] == 201)	/* semi-staggered E grid */
#define GDS_fEgrid(gds)		(gds[5] == 202) /* filled E grid */
#define GDS_ss2dEgrid(gds)	(gds[5] == 203) /* semi-staggered E grid 2 d*/
#define GDS_ss2dBgrid(gds)      (gds[5] == 205) /* semi-staggered B grid 2 d*/

#define GDS_has_dy(mode)	((mode) & 128)
#define GDS_LatLon_nx(gds)	((int) ((gds[6] << 8) + gds[7]))
#define GDS_LatLon_ny(gds)	((int) ((gds[8] << 8) + gds[9]))
#define GDS_LatLon_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_LatLon_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_LatLon_mode(gds)	(gds[16])
#define GDS_LatLon_La2(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_LatLon_Lo2(gds)	INT3(gds[20],gds[21],gds[22])

#define GDS_LatLon_dx(gds)      (gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_LatLon_dy(gds)      (gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_Gaussian_nlat(gds)  ((gds[25]<<8)+gds[26])

#define GDS_LatLon_scan(gds)	(gds[27])

#define GDS_Polar_nx(gds)	(gds[16] & 128 ? (UINT2(gds[6],gds[7])) : 0)
#define GDS_Polar_ny(gds)	(gds[16] & 128 ? (UINT2(gds[8],gds[9])) : 0)
#define GDS_Polar_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_Polar_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_Polar_mode(gds)	(gds[16])
#define GDS_Polar_Lov(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_Polar_scan(gds)	(gds[27])
#define GDS_Polar_Dx(gds)	INT3(gds[20], gds[21], gds[22])
#define GDS_Polar_Dy(gds)	INT3(gds[23], gds[24], gds[25])
#define GDS_Polar_pole(gds)	((gds[26] & 128) == 128)

#define GDS_Lambert_nx(gds)	((gds[6] << 8) + gds[7])
#define GDS_Lambert_ny(gds)	((gds[8] << 8) + gds[9])
#define GDS_Lambert_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_Lambert_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_Lambert_mode(gds)	(gds[16])
#define GDS_Lambert_Lov(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_Lambert_dx(gds)	INT3(gds[20],gds[21],gds[22])
#define GDS_Lambert_dy(gds)	INT3(gds[23],gds[24],gds[25])
#define GDS_Lambert_NP(gds)	((gds[26] & 128) == 0)
#define GDS_Lambert_scan(gds)   (gds[27])
#define GDS_Lambert_Latin1(gds)	INT3(gds[28],gds[29],gds[30])
#define GDS_Lambert_Latin2(gds)	INT3(gds[31],gds[32],gds[33])
#define GDS_Lambert_LatSP(gds)	INT3(gds[34],gds[35],gds[36])

/* bug found by Paul Schou 5/3/2011
   #define GDS_Lambert_LonSP(gds)	INT3(gds[37],gds[37],gds[37])
*/
#define GDS_Lambert_LonSP(gds)	INT3(gds[37],gds[38],gds[39])

#define GDS_ssEgrid_n(gds)	UINT2(gds[6],gds[7])
#define GDS_ssEgrid_n_dum(gds)  UINT2(gds[8],gds[9])
#define GDS_ssEgrid_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_ssEgrid_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_ssEgrid_mode(gds)	(gds[16])
#define GDS_ssEgrid_La2(gds)	UINT3(gds[17],gds[18],gds[19])
#define GDS_ssEgrid_Lo2(gds)	UINT3(gds[20],gds[21],gds[22])
#define GDS_ssEgrid_di(gds)	(gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_ssEgrid_dj(gds)	(gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_ssEgrid_scan(gds)	(gds[27])

#define GDS_fEgrid_n(gds)	UINT2(gds[6],gds[7])
#define GDS_fEgrid_n_dum(gds)   UINT2(gds[8],gds[9])
#define GDS_fEgrid_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_fEgrid_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_fEgrid_mode(gds)	(gds[16])
#define GDS_fEgrid_La2(gds)	UINT3(gds[17],gds[18],gds[19])
#define GDS_fEgrid_Lo2(gds)	UINT3(gds[20],gds[21],gds[22])
#define GDS_fEgrid_di(gds)	(gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_fEgrid_dj(gds)	(gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_fEgrid_scan(gds)	(gds[27])

#define GDS_ss2dEgrid_nx(gds)     UINT2(gds[6],gds[7])
#define GDS_ss2dEgrid_ny(gds)     UINT2(gds[8],gds[9])
#define GDS_ss2dEgrid_La1(gds)    INT3(gds[10],gds[11],gds[12])
#define GDS_ss2dEgrid_Lo1(gds)    INT3(gds[13],gds[14],gds[15])
#define GDS_ss2dEgrid_mode(gds)   (gds[16])
#define GDS_ss2dEgrid_La2(gds)    INT3(gds[17],gds[18],gds[19])
#define GDS_ss2dEgrid_Lo2(gds)    INT3(gds[20],gds[21],gds[22])
#define GDS_ss2dEgrid_di(gds)     (gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_ss2dEgrid_dj(gds)     (gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_ss2dEgrid_scan(gds)   (gds[27])

#define GDS_ss2dBgrid_nx(gds)     UINT2(gds[6],gds[7])
#define GDS_ss2dBgrid_ny(gds)     UINT2(gds[8],gds[9])
#define GDS_ss2dBgrid_La1(gds)    INT3(gds[10],gds[11],gds[12])
#define GDS_ss2dBgrid_Lo1(gds)    INT3(gds[13],gds[14],gds[15])
#define GDS_ss2dBgrid_mode(gds)   (gds[16])
#define GDS_ss2dBgrid_La2(gds)    INT3(gds[17],gds[18],gds[19])
#define GDS_ss2dBgrid_Lo2(gds)    INT3(gds[20],gds[21],gds[22])
#define GDS_ss2dBgrid_di(gds)     (gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_ss2dBgrid_dj(gds)     (gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_ss2dBgrid_scan(gds)   (gds[27]) 


#define GDS_Merc_nx(gds)	UINT2(gds[6],gds[7])
#define GDS_Merc_ny(gds)	UINT2(gds[8],gds[9])
#define GDS_Merc_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_Merc_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_Merc_mode(gds)	(gds[16])
#define GDS_Merc_La2(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_Merc_Lo2(gds)	INT3(gds[20],gds[21],gds[22])
#define GDS_Merc_Latin(gds)	INT3(gds[23],gds[24],gds[25])
#define GDS_Merc_scan(gds)	(gds[27])
#define GDS_Merc_dx(gds)        (gds[16] & 128 ? INT3(gds[28],gds[29],gds[30]) : 0)
#define GDS_Merc_dy(gds)        (gds[16] & 128 ? INT3(gds[31],gds[32],gds[33]) : 0)

/* rotated Lat-lon grid */

#define GDS_RotLL_nx(gds)	UINT2(gds[6],gds[7])
#define GDS_RotLL_ny(gds)	UINT2(gds[8],gds[9])
#define GDS_RotLL_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_RotLL_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_RotLL_mode(gds)	(gds[16])
#define GDS_RotLL_La2(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_RotLL_Lo2(gds)	INT3(gds[20],gds[21],gds[22])
#define GDS_RotLL_dx(gds)       (gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_RotLL_dy(gds)       (gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_RotLL_scan(gds)	(gds[27])
#define GDS_RotLL_LaSP(gds)	INT3(gds[32],gds[33],gds[34])
#define GDS_RotLL_LoSP(gds)	INT3(gds[35],gds[36],gds[37])
#define GDS_RotLL_RotAng(gds)	ibm2flt(&(gds[38]))

/* stretched Lat-lon grid */

#define GDS_StrLL_nx(gds)	UINT2(gds[6],gds[7])
#define GDS_StrLL_ny(gds)	UINT2(gds[8],gds[9])
#define GDS_StrLL_La1(gds)	INT3(gds[10],gds[11],gds[12])
#define GDS_StrLL_Lo1(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_StrLL_mode(gds)	(gds[16])
#define GDS_StrLL_La2(gds)	INT3(gds[17],gds[18],gds[19])
#define GDS_StrLL_Lo2(gds)	INT3(gds[20],gds[21],gds[22])
#define GDS_StrLL_dx(gds)       (gds[16] & 128 ? INT2(gds[23],gds[24]) : 0)
#define GDS_StrLL_dy(gds)       (gds[16] & 128 ? INT2(gds[25],gds[26]) : 0)
#define GDS_StrLL_scan(gds)	(gds[27])
#define GDS_StrLL_LoStrP(gds)	INT3(gds[32],gds[33],gds[34])
#define GDS_StrLL_LaStrP(gds)	INT3(gds[35],gds[36],gds[37])
#define GDS_StrLL_StrFactor(gds)	ibm2flt(&(gds[38]))

/* Triangular grid of DWD */
#define GDS_Triangular_ni2(gds)	INT2(gds[6],gds[7])
#define GDS_Triangular_ni3(gds)	INT2(gds[8],gds[9])
#define GDS_Triangular_ni(gds)	INT3(gds[13],gds[14],gds[15])
#define GDS_Triangular_nd(gds)  INT3(gds[10],gds[11],gds[12])

/* Harmonics data */
#define GDS_Harmonic_nj(gds)     ((int) (UINT2(gds[6],gds[7]))) 
#define GDS_Harmonic_nk(gds)     ((int) (UINT2(gds[8],gds[9]))) 
#define GDS_Harmonic_nm(gds)     ((int) (UINT2(gds[10],gds[11]))) 
#define GDS_Harmonic_type(gds)   (gds[12])
#define GDS_Harmonic_mode(gds)   (gds[13])

/* index of NV and PV */
#define GDS_PV(gds)		((gds[3] == 0) ? -1 : (int) gds[4] - 1)
#define GDS_PL(gds)		((gds[4] == 255) ? -1 : (int) gds[3] * 4 + (int) gds[4] - 1)

/*
 * get grid size from GDS
 *
 * added calculation of nxny of spectral data and clean up of triangular
 * grid nnxny calculation     l. kornblueh 
 * 7/25/03 wind fix Dusan Jovic
 * 9/17/03 fix scan mode
 */
extern int ec_large_grib, len_ec_bds;

int GDS_grid(unsigned char *gds, unsigned char *bds, int *nx, int *ny, 
             long int *nxny);

#define NCOL 15
void GDS_prt_thin_lon(unsigned char *gds);

/*
 * prints out wind rel to grid or earth
 */

static const char *scan_mode[8] = {
	"WE:NS",
	"NS:WE",

	"WE:SN",
	"SN:WE",

        "EW:NS",
	"NS:EW",

	"EW:SN",
	"SN:EW" };


void GDS_winds(unsigned char *gds, int verbose);

#include <code_tables/table_6.h>

template<RepresentationType>
struct GridDefinition;