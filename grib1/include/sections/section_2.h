#pragma once
#include "code_tables/table_6.h"
/* version 1.4.5 of grib headers  w. ebisuzaki */
/* this version is incomplete */
/* 5/00 - dx/dy or di/dj controlled by bit 1 of resolution byte */
/* 8/00 - dx/dy or di/dj for polar and lambert not controlled by res. byte */
/* Added headers for the triangular grid of the gme model of DWD
         Helmut P. Frank, 13.09.2001 */
/* Clean up of triangular grid properties access and added spectral information
         Luis Kornblueh, 27.03.2002 */
/* 8/08 - dx/dy (polar,lambert) controlled by bit 1 of resolution byte */
/* 5/11 Paul Schou: fixed GDS_Lambert_LonSP(gds) */
/* 6/11 Jeffery S. Smith Albers equal area projection */
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

STRUCT_BEG(GridDescriptionSection)
{
	unsigned char* buf_;
	// GridDataType data;
	// uint32_t section2Length DEF_STRUCT_VAL(0)
	// uint8_t numberOfVerticalCoordinateValues DEF_STRUCT_VAL(0)
	// uint8_t pvLocation DEF_STRUCT_VAL(0)
	// RepresentationType dataRepresentationType DEF_STRUCT_VAL(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR)
	// uint32_t PV DEF_STRUCT_VAL(0)
	//bool defined DEF_STRUCT_VAL(false)
	#ifdef __cplusplus
	GridDescriptionSection(unsigned char* buffer):buf_(buffer){}

	unsigned long section_length();
	RepresentationType get_representation_type();
	GridInfo define_grid();
	unsigned long get_number_vertical_coord_values();
	unsigned long get_PV();
	unsigned long get_PL();
	unsigned long number_values();
	#endif
}
STRUCT_END(GridDescriptionSection)

#ifndef __cplusplus
#include <stdint.h>

extern bool define_GDS(GridDescriptionSection* gds,char* buffer,size_t file_size);
unsigned long section_length(const GridDescriptionSection* data);
RepresentationType get_representation_type(const GridDescriptionSection* data);
GridInfo define_grid(const GridDescriptionSection* data,RepresentationType rep_t);
unsigned long get_number_vertical_coord_values(const GridDescriptionSection* data);
unsigned long get_PV(const GridDescriptionSection* data);
unsigned long get_PL(const GridDescriptionSection* data);
#endif