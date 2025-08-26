#pragma once

namespace API{
    enum TYPES{
        COMMON,
        #ifdef GRIB1API
        GRIB1,
        #endif
        #ifdef GRIB2API
        GRIB2,
        #endif
        #ifdef NETCDFAPI
        NETCDF,
        #endif
    };
}