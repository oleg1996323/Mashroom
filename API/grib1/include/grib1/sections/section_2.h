#pragma once
#include "grib1/code_tables.h"
#include "grib1/sections/def.h"
#include "grib1/sections/grid/def.h"
#include "grib1/sections/grid/grid.h"
#include <cstdint>
#include <fstream>
#include <vector>

#ifndef GRID_INFO
struct GridInfo;
#endif

struct GridDescriptionSection{
	unsigned char* buf_;
	GridDescriptionSection(unsigned char* buffer):buf_(buffer){}

	unsigned long section_length();
	uint8_t get_representation_type() const noexcept;
	GridInfo define_grid() const noexcept;
	unsigned long get_number_vertical_coord_values() const noexcept;
	unsigned long get_PV() const noexcept;
	unsigned long get_PL() const noexcept;
	long int nxny() const noexcept;
	long int nx() const noexcept;
	long int ny() const noexcept;
};