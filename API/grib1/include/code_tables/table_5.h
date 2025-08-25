#pragma once

enum TimeRange{
    FORECAST_P1 = 0,
    P1_0 = 1,
    BETWEEN_P1_P2 = 2,
    AVG_BETWEEN_P1_P2 = 3,
    ACCUM_P1_P2 = 4,
    DIFF_P2_P1 = 5,
    AVG_BETWEEN_SUB_P1_SUB_P2 = 6,
    AVG_BETWEEN_SUB_P1_PLUS_P2 = 7,
    P1_19_20 = 10,
    CLIMAT_MEAN_VALUE = 51,
    AVG_N_FORECAST_EACH_P1 = 113,
    ACCUM_N_FORECAST_EACH_P1 = 114,
    AVG_N_FORECAST_FIRST_P1 = 115,
    ACCUM_N_FORECAST_FIRST_P1 = 116
    //https://codes.ecmwf.int/grib/format/grib1/ctable/5/
};

/*
0	Forecast product valid for reference time + P1 (P1 > 0), or Uninitialized analysis product for reference time (P1 = 0), or Image product for reference time (P1 = 0)
1	Initialized analysis product for reference time (P1 = 0)
2	Product with a valid time ranging between reference time + P1 and reference time + P2
3	Average (reference time + P1 to reference time + P2)
4	Accumulation (reference time + P1 to reference time + P2) product considered valid at reference time + P2
5	Difference (reference time + P2 minus reference time + P1) product considered valid at reference time + P2
6	Average (reference time - P1 to reference time - P2)
7	Average (reference time - P1 to reference time + P2)
8-9	Reserved
10	P1 occupies octets 19 and 20; product valid at reference time + P1
11-50	Reserved
51	Climatological mean value: multiple year averages of quantities which are themselves means over some period of time (P2) less than a year.
    The reference time (R) indicates the date and time of the start of a period of time, given by R to R + P2, over which a mean is formed; N indicates
    the number of such period-means that are averaged together to form the climatological value, assuming that the N period-mean fields are separated by one year.
    The reference time indicates the start of the N-year climatology. If P1 = 0 then the data averaged in the basic interval P2 are assumed to be continuous, i.e.
    all available data are simply averaged together. If P1 = 1 (the unit of time - octet 18, Code table 4 - is not relevant here) then the data averaged together
    in the basic interval P2 are valid only at the time (hour, minute) given in the reference time, for all the days included in the P2 period. The units of P2
    are given by the contents of octet 18 and Code table 4.
52-112	Reserved
113	Average of N forecasts (or initialized analyses); each product has forecast period of P1 (P1 = 0 for initialized analyses); products have reference times at intervals of P2, beginning at the given reference time
114	Accumulation of N forecasts (or initialized analyses); each product has forecast period of P1 (P1 = 0 for initialized analyses); products have reference times at intervals of P2, beginning at the given reference time
115	Average of N forecasts, all with the same reference time; the first has a forecast period of P1, the remaining forecasts follow at intervals of P2
116	Accumulation of N forecasts, all with the same reference time; the first has a forecast period of P1, the remaining forecasts follow at intervals of P2
117	Average of N forecasts; the first has a forecast period of P1, the subsequent ones have forecast periods reduced from the previous one by an interval of P2;
    the reference time for the first is given in octets 13 to 17, the subsequent ones have reference times increased from the previous one by an interval of P2.
    Thus all the forecasts have the same valid time, given by the initial reference time + P1
118	Temporal variance, or covariance, of N initialized analyses; each product has forecast period of P1 = 0; products have reference times at intervals of P2, beginning at the given reference time
119	Standard deviation of N forecasts, all with the same reference time with respect to the time average of forecasts; the first forecast has a forecast period of P1, the remaining forecasts follow at intervals of P2
120-122	Reserved
123	Average of N uninitialized analyses, starting at the reference time, at intervals of P2
124	Accumulation of N uninitialized analyses, starting at the reference time, at intervals of P2
125	Standard deviation of N forecasts, all with the same reference time with respect to time average of the time tendency of forecasts; the first forecast has a forecast period of P1,
the remaining forecasts follow at intervals of P2
126-127	Reserved
128-254	Reserved for local use
255	Missing
*/