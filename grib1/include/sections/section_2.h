#pragma once
#include "code_tables/table_6.h"
#include "sections/section_3.h"
#include "sections/def.h"
#include "sections/grid/def.h"
#include "sections/grid/grid.h"
#include <cstdint>
#include <fstream>
#include <vector>

#ifndef GRID_INFO
typedef struct GridInfo;
#endif

struct GridDescriptionSection{
	unsigned char* buf_;
	#ifdef __cplusplus
	GridDescriptionSection(unsigned char* buffer):buf_(buffer){}

	unsigned long section_length();
	RepresentationType get_representation_type() const noexcept;
	GridInfo define_grid() const noexcept;
	unsigned long get_number_vertical_coord_values() const noexcept;
	unsigned long get_PV() const noexcept;
	unsigned long get_PL() const noexcept;
	long int nxny() const noexcept;
	long int nx() const noexcept;
	long int ny() const noexcept;
	#endif
};

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