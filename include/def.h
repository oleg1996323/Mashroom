#pragma once
#include <optional>
#include <chrono>
#include <code_tables/table_6.h>
#include "types/coord.h"
#include "types/data_common.h"

enum class TimeSeparation{
    HOUR,
    DAY,
    MONTH,
    YEAR
};
struct Properties{
    std::optional<CommonDataProperties> common_;
    std::optional<std::chrono::system_clock::time_point> from_date_;
    std::optional<std::chrono::system_clock::time_point> to_date_;
    std::optional<TimeSeparation> t_sep_;
    std::optional<RepresentationType> grid_type_;
    std::optional<Coord> position_;
};