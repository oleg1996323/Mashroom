#include "sections/grid/grid.h"

bool GridInfo::operator==(const GridInfo& other){
    if(rep_type!=other.rep_type)
        return false;
    else{
        switch (rep_type)
        {
        case LAT_LON_GRID_EQUIDIST_CYLINDR:
            return data.latlon==other.data.latlon;
            break;
        case ROTATED_LAT_LON:
            return data.rot_latlon == other.data.rot_latlon;
            break;
        case STRETCHED_LAT_LON:
            return data.str_latlon == other.data.str_latlon;
            break;
        case STRETCHED_AND_ROTATED_LAT_LON:
            return data.str_rot_latlon == other.data.str_rot_latlon;
            break;
        case GAUSSIAN:
            return data.gauss == other.data.gauss;
            break;
        default:
            throw std::runtime_error("Still unavailable");
            break;
        }
    }
}
bool GridInfo::operator!=(const GridInfo& other){
	return !(*this==other);
}