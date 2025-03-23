#pragma once

typedef struct{
    char identifier[4];
    unsigned msg_length;
    unsigned grib_edition;
}IndicatorSection;

extern void read_IS(IndicatorSection* section);