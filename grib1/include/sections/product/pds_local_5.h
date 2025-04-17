#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/4/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<4>{
    static unsigned char forecast_probability_number(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[49];
    }
    static unsigned char total_number_forecast_probabilities(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[50];
    }
    static unsigned char local_decimal_scale_factor(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[51];
    }
    static unsigned char threshold_indicator(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[52];
    }
    static short lower_threshold(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[54],pds.buf_[53]);
    }
    static short upper_threshold(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[56],pds.buf_[55]);
    }
};
#endif