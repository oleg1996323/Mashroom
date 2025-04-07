#include "sections/grid/grid.h"

bool GridInfo::operator==(const GridInfo& other){
    if(this->rep_type!=other.rep_type)
        return false;
    else{
        switch (this->rep_type)
        {
        case LAT_LON_GRID_EQUIDIST_CYLINDR:
            return this->data.latlon==other.data.latlon;
            break;
        
        default:
            break;
        }
    }
}
bool GridInfo::operator!=(const GridInfo& other){
	
}