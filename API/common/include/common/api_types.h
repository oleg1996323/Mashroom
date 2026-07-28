#pragma once
#include <optional>
#include <string>
#include <cstdint>

enum class API_T:int{
    COMMON,
    #ifdef GRIB1API
    GRIB1=1,
    #endif
    #ifdef GRIB2API
    GRIB2=2,
    #endif
    #ifdef NETCDFAPI
    NETCDF=3,
    #endif
};