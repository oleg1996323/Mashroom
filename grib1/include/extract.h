#pragma once
#include <stdio.h>
#include "def.h"
#include "sections/section_1.h"
#include "func.h"

typedef enum{
    ALL_TYPE_DATA,
    U10,
    V10,
    T2,
    NSSS,
    INSS,
    IEWS,
    EWSS,
    U100,
    V100,
    MLSPR
}TypeData;



extern GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos);
extern TaggedValues extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const char* ffrom,long int count, long unsigned pos);
extern ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos);