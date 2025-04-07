#pragma once
#include "types/rect.h"
#include "aux_code/def.h"
#include "types/grid.h"

#ifndef RECT_TYPE
#include_next "types/rect.h"
#endif

#define UNDEF_GRID_VALUE -99999999
[[deprecated]]
STRUCT_BEG(GridData)
{
	Rect bound DEF_STRUCT_VAL(Rect())
	float dy DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
	float dx DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
}
STRUCT_END(GridData)

#ifndef __cplusplus
#define GridData(...) ((GridData){.bound = Rect(),\
                            .dy = UNDEF_GRID_VALUE,\
                            .dx = UNDEF_GRID_VALUE,\
                            .nx = UNDEF_GRID_VALUE,\
                            .ny = UNDEF_GRID_VALUE,\
                            .nxny = UNDEF_GRID_VALUE,\
                            ## __VA_ARGS__})
#endif

extern bool point_in_grid(const GridData*, const Coord point);