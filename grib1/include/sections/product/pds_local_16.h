#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/2/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<16>{
    // /Ensemble member number. Control forecast is number 0, perturbed forecasts 1-nn.
    static unsigned short perturb_number(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16Number(pds.buf_);
    }
    //System number. The "scientific version" number.
    //0 = RD experiment, 1 - 65534 = operational version number, 65535 = missing
    static unsigned short system_number(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16SysNum(pds.buf_);
    }
    //Method number. Distinguishes scientifically different forecast ensembles (eg different calibration/bias correction)
    //0 = control integration (ie without data assimilation), 1 - 65534 = operational version number
    static unsigned short method_number(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16MethodNum(pds.buf_);
    }
    //Verifying month (in format YYYYMM)
    static unsigned long verif_month(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16VerfMon(pds.buf_);
    }
    //Averaging period (eg 6-hour, 24-hour)
    static unsigned char ave_period(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16AvePeriod(pds.buf_);
    }
    //Forecast month
    static unsigned short forecast_month(const PDSLocal<ECMWF>& pds) noexcept{
        return PDS_Ec16FcstMon(pds.buf_);
    }
    //Number of forecasts in ensemble
    static unsigned short number_forecast_ensemble(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes<2>(pds.buf_[62],pds.buf_[63]);
    }
};
#else
    // /Ensemble member number. Control forecast is number 0, perturbed forecasts 1-nn.
    static unsigned short perturb_number(const PDSLocal_ECMWF_* pds){
        return PDS_Ec16Number(pds->buf_);
    }
    //System number. The "scientific version" number.
    //0 = RD experiment, 1 - 65534 = operational version number, 65535 = missing
    static unsigned short system_number(const PDSLocal<ECMWF>& pds){
        return PDS_Ec16SysNum(pds.buf_);
    }
    //Method number. Distinguishes scientifically different forecast ensembles (eg different calibration/bias correction)
    //0 = control integration (ie without data assimilation), 1 - 65534 = operational version number
    static unsigned short method_number(const PDSLocal<ECMWF>& pds){
        return PDS_Ec16MethodNum(pds.buf_);
    }
    //Verifying month (in format YYYYMM)
    static unsigned long verif_month(const PDSLocal<ECMWF>& pds){
        return PDS_Ec16VerfMon(pds.buf_);
    }
    //Averaging period (eg 6-hour, 24-hour)
    static unsigned char ave_period(const PDSLocal<ECMWF>& pds){
        return PDS_Ec16AvePeriod(pds.buf_);
    }
    //Forecast month
    static unsigned short forecast_month(const PDSLocal<ECMWF>& pds){
        return PDS_Ec16FcstMon(pds.buf_);
    }
    //Number of forecasts in ensemble
    static unsigned short number_forecast_ensemble(const PDSLocal<ECMWF>& pds){
        return read_bytes<2>(pds.buf_[62],pds.buf_[63]);
    }
#endif