#pragma once
#include <stdio.h>
#include "def.h"
#include "sections/section_1.h"
#include "types/grid.h"
#include "types/tagged_vals.h"
#include "types/data_info.h"
#include <unordered_set>

typedef struct{
    int32_t time_date = -1; //in epoque
    float data_name = UNDEFINED;
    uint8_t parameter = 0;
    Organization center = WMO;
    uint8_t subcenter = 0;

    
}ExtractedData;

template<>
class std::hash<ExtractedData>{
    size_t operator()(const ExtractedData& data){
        return std::hash<int64_t>{}(static_cast<int64_t>(data.time_date)<<32|static_cast<int64_t>(data.parameter)<<16|static_cast<int64_t>(data.center)<<8|data.subcenter);
    }
};

#ifdef __cplusplus
std::unordered_set<ExtractedData> extract(const Date& dfrom, const Date& dto,const Coord& coord, TimeFrame fcst_unit,Organization center, uint8_t subcenter,const char* ffrom);
#else
extern GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos);
extern TaggedValues extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const char* ffrom,long int count, long unsigned pos);
extern ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos);
#endif