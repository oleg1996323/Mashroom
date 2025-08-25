#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/1/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<1>{
    //Set to 0 for MARS labeling (simple labeling).
    //When element 40 = 1035, this is the ensemble
    //forecast number. When element 39 = 10, this is
    //the control forecast number and is set to 0.
    //When element 39 = 11, this is the perturbed
    //forecast number in some range 1-nn. Positive
    //perturbations have odd numbers and negative
    //have even numbers. They go in pairs (1,2), (3,4) etc.
    //When element 39 = 17, this is an ensemble mean and
    //the number is set to 0. When element 39 = 18,
    //this is an ensemble standard deviation and
    //the number is set to 0.
    static unsigned char perturb_number(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[49];
    }
    //Set to 0, if not an ensemble forecast. Otherwise,
    //the total number of forecasts in an ensemble;
    //the number includes the control forecast.
    static unsigned char number_forecast_ensemble(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[50];
    }
};
#endif