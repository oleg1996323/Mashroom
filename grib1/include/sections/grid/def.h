#pragma once
#include "sections/def.h"

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
// #define GDS_LatLon_nx(gds)	((int) ((gds[6] << 8) + gds[7]))
// #define GDS_LatLon_ny(gds)	((int) ((gds[8] << 8) + gds[9]))
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

#include "code_tables/table_7.h"
#include "code_tables/table_8.h"
#pragma pack(push,1)

template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::ROTATED_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved_29_32[3];
};

template<>
struct GridDefinition<RepresentationType::MERCATOR>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t latin;
    char reserved_27;
    ScanMode scan_mode;
    uint32_t dy:24;
    uint32_t dx:24;
    uint8_t reserved_35_42[8];
};

template<>
struct GridDefinition<RepresentationType::GAUSSIAN>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::GNOMONIC>{
    
};

template<>
struct GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::LAMBERT>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::MILLERS_CYLINDR>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::SIMPLE_POLYCONIC>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::SPACE_VIEW>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

template<>
struct GridDefinition<RepresentationType::UTM>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};

#pragma pack(pop)

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include "code_tables/table_6.h"
void define_GDS();
template<typename T>
T define_grid(RepresentationType rep_t,char* buffer, size_t file_size){
    if(!rep_t<256 || !is_representation[rep_t])
        throw std::invalid_argument("Invalid representation type.");
    else{
        switch (rep_t)
        {
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
            /* code */
            break;
        
        default:
            break;
        }
    }
}
#else
#include <stdint.h>

extern void define_GDS()
#endif