#include <stdio.h>
#include "def.h"
#include "sections/section_2.h"
#include "sections/section_1.h"

#ifndef __cplusplus
extern bool define_GDS(GridDescriptionSection* gds,char* buffer,size_t file_size);
#endif

#ifdef __cplusplus
#include <fstream>
#include <vector>
unsigned long GridDescriptionSection::section_length(){
	return read_bytes<3>(buf_[0],buf_[1],buf_[2]);
}
RepresentationType GridDescriptionSection::get_representation_type(){
	return (RepresentationType)-1;
}
GridInfo GridDescriptionSection::define_grid(){
    if(!get_representation_type()<256 || !is_representation[get_representation_type()])
        throw std::invalid_argument("Invalid representation type.");
    else
        return GridInfo(GridDataType(buf_,get_representation_type()),get_representation_type());
}
unsigned long GridDescriptionSection::get_number_vertical_coord_values(){
	return GDS_NV(buf_);
}
unsigned long GridDescriptionSection::get_PV(){
	return GDS_PV(buf_);
}
unsigned long GridDescriptionSection::get_PL(){
	return GDS_PL(buf_);
}
unsigned long GridDescriptionSection::number_values(){
	switch ((get_representation_type()<256 && is_representation[get_representation_type()])?
			get_representation_type():-1)
        {
		case -1:
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
			throw std::runtime_error("Still not available");
            break;
		case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
        case RepresentationType::GAUSSIAN :{
			int i, d, ix, iy, pl;
			long int isum;
		
			ix = GDS_LatLon_nx(buf_);
			iy = GDS_LatLon_ny(buf_);
			long int nxny = ix * iy;
		
			/* thin grid */
		
			if (GDS_Gaussian(buf_) || GDS_LatLon(buf_)) {
			if (ix == 65535) {
				ix = -1;
				/* reduced grid */
				isum = 0;
				pl = GDS_PL(buf_);
				for (i = 0; i < iy; i++) {
					isum += buf_[pl+i*2]*256 + buf_[pl+i*2+1];
				}
				return isum;
			}
			return 0;
			}
            break;
		}
		/*
		return GDS_Triangular_nd(buf_) * 
				(GDS_Triangular_ni(buf_) + 1) * (GDS_Triangular_ni(buf_) + 1);
			break;
		*/
        default:
			throw std::runtime_error("Still not available");
            break;
        }
}

#else
#include <stdint.h>

extern void define_GDS()
#endif