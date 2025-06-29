#include <stdio.h>
#include <iostream>
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
	return read_bytes<3,false>(&buf_[0]);
}
uint8_t GridDescriptionSection::get_representation_type() const noexcept{
	return (uint8_t)buf_[5];
}
GridInfo GridDescriptionSection::define_grid() const noexcept{
	GridInfo grid;
	grid.data.emplace_by_id(static_cast<RepresentationType>(get_representation_type()),buf_);
	return grid;
}
unsigned long GridDescriptionSection::get_number_vertical_coord_values() const noexcept{
	return GDS_NV(buf_);
}
unsigned long GridDescriptionSection::get_PV() const noexcept{
	return GDS_PV(buf_);
}
unsigned long GridDescriptionSection::get_PL() const noexcept{
	return GDS_PL(buf_);
}
long int GridDescriptionSection::nxny() const noexcept{
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
			std::cerr<<"Still not available"<<std::endl;
			exit(1);
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
			else{
				std::cerr<<"Internal error"<<std::endl;
				exit(1);
			}
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

long int GridDescriptionSection::nx() const noexcept{
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
			std::cerr<<"Still not available"<<std::endl;
			exit(1);
            break;
		case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
        case RepresentationType::GAUSSIAN :{
			return GDS_LatLon_nx(buf_);
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
long int GridDescriptionSection::ny() const noexcept{
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
			std::cerr<<"Still not available"<<std::endl;
			exit(1);
            break;
		case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
        case RepresentationType::GAUSSIAN :{
			return GDS_LatLon_ny(buf_);
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

#else
#include <stdint.h>

extern void define_GDS()
#endif