#include "types/grid.h"
#include "math.h"

bool point_in_grid(const GridData* grid, const Coord pos){
	if(!point_in_rect(&grid->bound,pos) || fmod((double)(grid->bound.y1-pos.lat_)/grid->dy,1)!=0 || fmod((double)(grid->bound.x2-pos.lon_)/grid->dx,1)!=0)
		return false;
	else return true;
}