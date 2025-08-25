#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/2/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<2>{
    static unsigned char cluster_number(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[49];
    }
    static unsigned char total_number_of_clusters(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[50];
    }
    static unsigned char clustering_method(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[52];
    }
    static unsigned short start_time_step(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[53],pds.buf_[54]);
    }
    static unsigned short end_time_step(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[55],pds.buf_[56]);
    }
    static unsigned char north_lat_of_domain(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[57],pds.buf_[58],pds.buf_[59]);
    }
    static unsigned char west_long_of_domain(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[60],pds.buf_[61],pds.buf_[62]);
    }
    static unsigned char south_lat_of_domain(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[63],pds.buf_[64],pds.buf_[65]);
    }
    static unsigned char east_long_of_domain(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[66],pds.buf_[67],pds.buf_[68]);
    }
    static unsigned char operational_forecast_cluster(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[69];
    }
    static unsigned char control_forecast_in_cluster(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[70];
    }
    static unsigned char number_of_forecast_in_cluster(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[71];
    }
    static unsigned char* ensemble_forecast_numbers(const PDSLocal<ECMWF>& pds) noexcept{
        return &pds.buf_[73];
    }
};
#endif