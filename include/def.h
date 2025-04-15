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
using namespace std::chrono_literals;
using namespace std::chrono;
struct Properties{
    std::optional<CommonDataProperties> common_;
    system_clock::time_point from_date_ = system_clock::time_point(sys_days(1970y/1/1));
    system_clock::time_point to_date_ = system_clock::now();
    std::optional<RepresentationType> grid_type_;
    std::optional<Coord> position_;
    TimeSeparation t_sep_ = TimeSeparation::DAY;
};