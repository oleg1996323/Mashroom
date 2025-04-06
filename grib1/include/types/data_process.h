#pragma once
#include <vector>
#include <algorithm>
#include "aux_code/def.h"
#include "types/date.h"
#include "types/grid.h"
#include "types/data_suite.h"
#include "code_tables/table_0.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "code_tables/table_4.h"

STRUCT_BEG(ValuesGrid)
{
    int nx;
    int ny;
    #ifdef __cplusplus
    std::vector<float> values;
    #else
    float* values;
    #endif
}
STRUCT_END(ValuesGrid)

STRUCT_BEG(ValuesTimeRange)
{
    Date from;
    Date to;
    Coord coord;//TODO:
    #ifdef __cplusplus
    std::vector<float> val;
    #else
    size_t sz;
    size_t capacity;
    float* val;
    #endif
    unsigned char time_increment;
    TimeFrame tf_;
    #ifdef __cplusplus
    ValuesTimeRange():time_increment(0),tf_(TimeFrame::MINUTE){}
    #endif
}
STRUCT_END(ValuesTimeRange)

#ifndef __cplusplus
#define ValuesTimeRange(...) (ValuesTimeRange){.from=Date(),.to=Date(),.coord=Coord(),.sz=0,.capacity=0,.val=NULL,.time_increment=0,.tf_,## __VA_ARGS__}
#endif

STRUCT_BEG(DataTimeRange)
{
    #ifdef __cplusplus
    std::vector<ValuesTimeRange> values;
    #else
    size_t sz;
    size_t capacity;
    ValuesTimeRange* values;
    #endif
    unsigned char center;
    unsigned char subcenter;
    unsigned char parameter;
    #ifdef __cplusplus
    DataTimeRange():center(0),subcenter(0),parameter(0){}
    #endif
}
STRUCT_END(DataTimeRange)

#ifndef __cplusplus
#define DataTimeRange(...) (DataTimeRange){.sz=0,.capacity=0,.values=NULL,.center=0,.subcenter=0,.parameter=0,## __VA_ARGS__}
#endif
STRUCT_BEG(TaggedDataTimeRange)
{
    #ifdef __cplusplus
    std::vector<DataTimeRange> vals;
    TaggedDataTimeRange(){}
    #else
    size_t sz;
    size_t capacity;
    DataTimeRange* vals;
    #endif
}
STRUCT_END(TaggedDataTimeRange)

#ifndef __cplusplus
#define TaggedDataTimeRange(...) (TaggedDataTimeRange){.sz=0,.capacity=0,.vals=NULL,## __VA_ARGS__}
#define EXTRACT_DATA(...) ((ExtractData) { .date = Date(), .bound = Rect(), .type = ALL_TYPE_DATA, ##__VA_ARGS__ })
#define VALUESGRID(...) ((ValuesGrid) { .nx = -1, .ny = -1, .values = NULL, ##__VA_ARGS__ })
DataTimeRange* find_data_time_range_by_tag(TaggedDataTimeRange* tagged_vals, const char* tag);
extern void delete_tagged_data_time_range(TaggedDataTimeRange tag_vals);
extern void delete_tagged_data_time_range_by_ptr(TaggedDataTimeRange* tag_vals);
#endif