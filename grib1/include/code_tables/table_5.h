#pragma once

enum TimeRange{
    FORECAST_P1 = 0,
    P1_0 = 1,
    BETWEEN_P1_P2 = 2,
    AVG_BETWEEN_P1_P2 = 3,
    ACCUM_P1_P2 = 4,
    DIFF_P2_P1 = 5,
    AVG_BETWEEN_SUB_P1_SUB_P2 = 6,
    AVG_BETWEEN_SUB_P1_P2 = 7,
    P1_19_20 = 10,
    CLIMAT_MEAN_VALUE = 51,
    // AVG_N_FORECAST = 113,
    // ACCUM_N_FORECAST = 114,
    //https://codes.ecmwf.int/grib/format/grib1/ctable/5/
};