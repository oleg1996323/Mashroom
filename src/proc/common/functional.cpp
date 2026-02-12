#include "proc/common/functional.h"

TimePeriodRounded round_by_time_diff(const DateTimeDiff& diff, utc_tp_t<std::chrono::seconds> tp) {
    
    if(diff.years_>0)
        return std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(tp)).year();
    else if(diff.months_>0){
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(tp));
        return std::chrono::year_month(ymd.year(),ymd.month());
    }
    else if(diff.days_>0)
        return std::chrono::floor<std::chrono::days>(tp);
    else if(diff.hours_>0)
        return std::chrono::floor<std::chrono::hours>(tp);
    else if(diff.minutes_>0)
        return std::chrono::floor<std::chrono::minutes>(tp);
    else if(diff.seconds_>0)
        return std::chrono::floor<std::chrono::seconds>(tp);
    else{
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(tp));
        return std::chrono::year_month(ymd.year(),ymd.month());
    }
}