#pragma once
#include "data/def.h"
#include "data/common_data_properties.h"
#include "data/sublimed_info.h"
#include "data/info.h"
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_set>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>
#include "types/time_interval.h"
#include "types/coord.h"
#include "definitions/def.h"
#include "definitions/path_process.h"
#include "data/datastruct.h"
template<Data_t TYPE,Data_f FORMAT>
struct DataStruct;

template<Data_t DATA_T,Data_f DATA_F>
struct __Data_type__{
    static constexpr Data_f format_type() noexcept{
        return DATA_F;
    }
    static constexpr Data_t data_type() noexcept{
        return DATA_T;
    }
};
template<Data_t TYPE,Data_f FORMAT>
using FoundSublimedDataInfo = std::map<CommonDataProperties<TYPE,FORMAT>, std::vector<SublimedDataInfoStruct<TYPE,FORMAT>>>;