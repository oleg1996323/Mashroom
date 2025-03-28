#pragma once
#include "types/rect.h"
#include "aux_code/def.h"

#define UNDEF_GRID_VALUE -99999999
STRUCT_BEG(GridData)
{
	Rect bound DEF_STRUCT_VAL(Rect())
	double dy DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
	double dx DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
	int nx DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
	int ny DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
	long int nxny DEF_STRUCT_VAL(UNDEF_GRID_VALUE)
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