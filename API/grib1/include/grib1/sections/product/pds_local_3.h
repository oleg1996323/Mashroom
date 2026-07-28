#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/3/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<3>{
    //Band: 0 = first infrared band
    //1 = second infrared band
    //10 = first visible band
    //20 = water vapour
    static unsigned char band(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[49];
    }
    //Function code (interpretation of pixel value):
    //0 = value is pixel value
    //1 = value is temperature in degrees K,
    //and is 145 + pixel value
    //2 = value is temperature in degrees K,
    //and is the pixel value
    //255 = translation table follows
    static unsigned char function_code(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[50];
    }
};
#endif