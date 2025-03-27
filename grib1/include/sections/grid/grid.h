#pragma once

#include "code_tables/table_7.h"
#include "code_tables/table_8.h"
#include "albers.h"
#include "lambert.h"
#include "lat_lon.h"
#include "gnomonic.h"
#include "gaussian.h"
#include "mercator.h"
#include "millers.h"
#include "polar.h"
#include "polyconic.h"
#include "space_view.h"
#include "utm.h"

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include "code_tables/table_6.h"
void define_GDS();
template<typename T>
T define_grid(char* buffer, size_t file_size){
    if(!rep_t<256 || !is_representation[rep_t])
        throw std::invalid_argument("Invalid representation type.");
    else{
        return 
    }
}
#else
#include <stdint.h>

extern void define_GDS()
#endif