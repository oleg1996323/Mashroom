#pragma once
#include <variant>
#include "OsterLib/types/time_interval.h"

using TimePeriodRounded =   std::variant<std::chrono::year,
                            std::chrono::year_month,
                            utc_tp_t<std::chrono::days>,
                            utc_tp_t<std::chrono::hours>,
                            utc_tp_t<std::chrono::minutes>,
                            utc_tp_t<std::chrono::seconds>>;

TimePeriodRounded round_by_time_diff(const DateTimeDiff& diff, utc_tp_t<std::chrono::seconds> tp);