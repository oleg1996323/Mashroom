#include "sections/grid/grid.h"

bool GridInfo::operator==(const GridInfo& other) const{
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
bool GridInfo::operator!=(const GridInfo& other) const{
	return !(*this==other);
}

#include <cmath>
#include <iostream>
bool pos_in_grid(const Coord& pos, const GridInfo& grid) noexcept{
	switch (grid.rep_type)
        {
        case RepresentationType::ALBERS_EQUAL_AREA :
		case RepresentationType::ROTATED_GAUSSIAN_LAT_LON :
		case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON :
		case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON :
        case RepresentationType::ROTATED_LAT_LON:
        case RepresentationType::STRETCHED_LAT_LON :
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON :
        case RepresentationType::LAMBERT :
        case RepresentationType::MERCATOR :
        case RepresentationType::MILLERS_CYLINDR :
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL :
        case RepresentationType::POLAR_STEREOGRAPH_PROJ :
        case RepresentationType::SIMPLE_POLYCONIC :
        case RepresentationType::GNOMONIC :
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
        case RepresentationType::UTM :
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS :
        case RepresentationType::SPACE_VIEW :
		case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
		case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
			std::cerr<<"Still not available"<<std::endl;
            exit(1);
            break;
		case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
            if(pos.lat_<grid.data.latlon.y2 || pos.lat_>grid.data.latlon.y1 || pos.lon_<grid.data.latlon.x1 || pos.lon_>grid.data.latlon.x2)
                return false;
            return true;
            break;
        case RepresentationType::GAUSSIAN :{
			if(fmod((double)(grid.data.gauss.y1-pos.lat_)/
						((grid.data.gauss.y1-grid.data.gauss.y2)/
						grid.data.gauss.ny),1)!=0 || 
						fmod((double)(grid.data.gauss.x2-pos.lon_)/
						((grid.data.gauss.x2-grid.data.gauss.x1)/
						grid.data.gauss.nx),1)!=0)
				return false;
			else return true;
            break;
		}
		/*
		return GDS_Triangular_nd(buf_) * 
				(GDS_Triangular_ni(buf_) + 1) * (GDS_Triangular_ni(buf_) + 1);
			break;
		*/
        default:
            std::cerr<<"Still not available"<<std::endl;
            exit(1);
            break;
        }
}

int value_by_raw(const Coord& pos, const GridInfo& grid) noexcept{
    if(is_correct_pos(&pos))
    switch (grid.rep_type)
        {
        case RepresentationType::ALBERS_EQUAL_AREA :
		case RepresentationType::ROTATED_GAUSSIAN_LAT_LON :
		case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON :
		case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON :
        case RepresentationType::ROTATED_LAT_LON:
        case RepresentationType::STRETCHED_LAT_LON :
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON :
        case RepresentationType::LAMBERT :
        case RepresentationType::MERCATOR :
        case RepresentationType::MILLERS_CYLINDR :
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL :
        case RepresentationType::POLAR_STEREOGRAPH_PROJ :
        case RepresentationType::SIMPLE_POLYCONIC :
        case RepresentationType::GNOMONIC :
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
        case RepresentationType::UTM :
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS :
        case RepresentationType::SPACE_VIEW :
		case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
		case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
        case RepresentationType::GAUSSIAN :
			return UNDEFINED;
            break;
		case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
            return (pos.lon_-grid.data.latlon.x1)/grid.data.latlon.dx+grid.data.latlon.nx*(pos.lat_-grid.data.latlon.y2)/grid.data.latlon.dy;
            break;
        default:
			return UNDEFINED;
            break;
    }
    else return UNDEFINED;
}