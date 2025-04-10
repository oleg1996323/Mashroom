#pragma once
#include <stdio.h>
#include "def.h"
#include "sections/section_1.h"
#include "types/grid.h"
#include <unordered_map>
#include "types/data_common.h"

typedef struct{
    int32_t time_date = -1; //in epoque
    float value = UNDEFINED;
}ExtractedValue;

template<>
class std::hash<ExtractedValue>{
    size_t operator()(const ExtractedValue& data){
        return std::hash<int64_t>{}(static_cast<int64_t>(data.time_date)<<32|static_cast<int64_t>(data.value));
    }
};

#ifdef __cplusplus
#include <filesystem>
namespace fs = std::filesystem;
std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>> extract(const Date& dfrom, const Date& dto,const Coord& coord,const fs::path& ffrom,
                                                                    std::optional<TimeFrame> fcst_unit={},std::optional<Organization> center={}, std::optional<uint8_t> subcenter={});
#else
extern GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos);
extern TaggedValues extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const char* ffrom,long int count, long unsigned pos);
extern ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos);
#endif