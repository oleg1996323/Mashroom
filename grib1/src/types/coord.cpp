#include "types/coord.h"

bool is_correct_pos(const Coord* pos){
	if(pos->lon_>=0 && pos->lon_<=180 && pos->lat_<=90 && pos->lat_>=-90)
		return true;
	else return false;
}