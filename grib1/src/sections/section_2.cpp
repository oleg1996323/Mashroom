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

#else
#include <stdint.h>

extern void define_GDS()
#endif