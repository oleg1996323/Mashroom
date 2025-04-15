#pragma once
#include <vector>
#include <algorithm>
#include "aux_code/def.h"
#include "types/date.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "data_common.h"

struct ValuesGrid
{
    std::vector<float> values;
    int nx;
    int ny;
};
#include <chrono>
struct ValuesTimeRange
{
    std::chrono::system_clock::time_point from;
    std::chrono::system_clock::time_point to;
    Coord coord;
    std::vector<float> val;
    unsigned char time_increment;
    TimeFrame tf_;
    ValuesTimeRange():time_increment(0),tf_(TimeFrame::MINUTE){}
};
struct DataTimeRange
{
    std::vector<ValuesTimeRange> values;
    unsigned char center;
    unsigned char subcenter;
    unsigned char parameter;
    DataTimeRange():center(0),subcenter(0),parameter(0){}
};
struct TaggedDataTimeRange
{
    std::vector<DataTimeRange> vals;
    TaggedDataTimeRange(){}
};