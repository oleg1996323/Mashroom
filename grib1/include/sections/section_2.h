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

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

STRUCT_BEG(GridDescriptionSection)
{
	uint32_t section2Length = 0;
	uint8_t numberOfVerticalCoordinateValues = 0;
	uint8_t pvLocation = 0;
	RepresentationType dataRepresentationType;
	
}
STRUCT_END(GridDescriptionSection)

#ifdef __cplusplus
bool define_GDS(GridDescriptionSection& gds,char* buffer,size_t file_size){
	uint32_t section2Length = read_bytes<3>(buffer[0],buffer[1],buffer[2]);
}
#else
extern bool define_GDS(GridDescriptionSection* gds,char* buffer,size_t file_size);
#endif

/*
 * get grid size from GDS
 *
 * added calculation of nxny of spectral data and clean up of triangular
 * grid nnxny calculation     l. kornblueh 
 * 7/25/03 wind fix Dusan Jovic
 * 9/17/03 fix scan mode
 */
extern int ec_large_grib, len_ec_bds;

int GDS_LatLon_nx(unsigned char* gds);
int GDS_LatLon_ny(unsigned char* gds);

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

#ifdef __cplusplus
#include <fstream>
#include <vector>
bool read_GDS(std::ifstream& file, GridDescriptionSection& data){
	file.clear();
	std::vector<char> buf;
	buf.resize(7);
	if(!file.read(buf.data(),buf.size())) return false;
	data.section2Length = UINT3(buf[0],buf[1],buf[2]);
	if(data.section2Length==0) return false;
		
}
#endif