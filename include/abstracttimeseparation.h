#pragma once
#include <types/time_interval.h>

class AbstractTimeSeparation{  
    protected:
    virtual void __set_offset_time_interval__()=0;
    TimeOffset diff_time_interval_ = {years(0),months(1),days(0),hours(0),minutes(0),std::chrono::seconds(0)};
    utc_tp __get_next_period__(utc_tp current_time) {
        using namespace std::chrono;
        auto ymd = year_month_day(floor<days>(current_time));
        ymd+=diff_time_interval_.years_;
        ymd+=diff_time_interval_.months_;
        auto sd = utc_tp(sys_days(ymd));
        sd+=diff_time_interval_.days_+diff_time_interval_.hours_;
        return sd;
    }
    utc_tp __get_begin_period__(utc_tp current_time, utc_tp from_initial_interval){
        using namespace std::chrono;
        auto ymd_current = year_month_day(floor<days>(current_time));
        auto ymd_initial = year_month_day(floor<days>(from_initial_interval));
        years year;
        if(diff_time_interval_.seconds_>std::chrono::seconds(0))

        if(diff_time_interval_.years_!=years(0)){
            year = ((ymd_current.year()-ymd_initial.year())/diff_time_interval_.years_)*diff_time_interval_.years_;
            if(year==years(0))
                return from_initial_interval;
        }
        months month;
        if(diff_time_interval_.months_!=months(0)){
            if(diff_time_interval_.months_>months(11)){
                diff_time_interval_.years_+=years(diff_time_interval_.months_/months(12));
                diff_time_interval_.months_=months(diff_time_interval_.months_%months(12));
            }
            if(diff_time_interval_.years_!=years(0)){
                year = ((ymd_current.year()-ymd_initial.year())/diff_time_interval_.years_)*diff_time_interval_.years_;
                if(year==years(0))
                    return from_initial_interval;
            }
            month=(ymd_current.month()-ymd_initial.month())/diff_time_interval_.months_*diff_time_interval_.months_;
            if(month==months(0))
                return from_initial_interval;
        }
        days day;
        if(diff_time_interval_.days_>days(0)){
            day=(sys_days(floor<days>(current_time))-sys_days(ymd_initial+year+month))/diff_time_interval_.days_*diff_time_interval_.days_;
            if(day==days(0))
                return from_initial_interval;
        }
        utc_tp result = sys_days(ymd_initial+year+month)+day;
        hours hour;
        if(diff_time_interval_.hours_>hours(0)){
            hour=(current_time-result)/diff_time_interval_.hours_*diff_time_interval_.hours_;
            if(hour==hours(0))
                return from_initial_interval;
            else result+=hour;
        }
        minutes minute;
        if(diff_time_interval_.minutes_>minutes(0)){
            minute=(current_time-result)/diff_time_interval_.minutes_*diff_time_interval_.minutes_;
            if(minute==minutes(0))
                return from_initial_interval;
            else result+=minute;
        }
        std::chrono::seconds second;
        if(diff_time_interval_.seconds_>std::chrono::seconds(0)){
            second=(current_time-result)/diff_time_interval_.seconds_*diff_time_interval_.seconds_;
            if(second==std::chrono::seconds(0))
                return from_initial_interval;
            else result+=second;
        }
        return result;
    }
    public:
    void set_offset_time_interval(const TimeOffset& diff){
        diff_time_interval_ = diff;
        if(diff_time_interval_.seconds_>std::chrono::seconds(0)){
            if(diff_time_interval_.seconds_>std::chrono::seconds(59)){
                diff_time_interval_.days_=days(diff_time_interval_.seconds_/std::chrono::seconds(86400));
                diff_time_interval_.seconds_%=86400;
                diff_time_interval_.hours_=hours(diff_time_interval_.seconds_/std::chrono::seconds(3600));
                diff_time_interval_.seconds_%=3600;
                diff_time_interval_.minutes_=minutes(diff_time_interval_.seconds_/std::chrono::seconds(60));
                diff_time_interval_.seconds_%=60;
            }
        }
        if(diff_time_interval_.minutes_>minutes(0)){
            if(diff_time_interval_.minutes_>=std::chrono::hours(1)){
                diff_time_interval_.days_=days(diff_time_interval_.minutes_/days(1));
                diff_time_interval_.minutes_%=days(1);
                diff_time_interval_.hours_=hours(diff_time_interval_.minutes_/hours(1));
                diff_time_interval_.minutes_%=hours(1);
            }
        }
        if(diff_time_interval_.hours_>hours(0)){
            if(diff_time_interval_.hours_>=std::chrono::days(1)){
                diff_time_interval_.days_=days(diff_time_interval_.hours_/days(1));
                diff_time_interval_.hours_%=days(1);
            }
        }
        __set_offset_time_interval__();
    }
    const TimeOffset& get_offset_time_interval() const{
        return diff_time_interval_;
    }
};