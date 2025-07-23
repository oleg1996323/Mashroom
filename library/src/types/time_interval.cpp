#include "types/time_interval.h"
TimeOffset::TimeOffset(years y,months mo,days d,hours h,minutes m,std::chrono::seconds s) noexcept:
    years_(y),
    months_(mo),
    days_(d),
    hours_(h),
    minutes_(m),
    seconds_(s)
{
    if(seconds_>std::chrono::seconds(0)){
        if(seconds_>std::chrono::seconds(59)){
            days_=days(seconds_/std::chrono::seconds(86400));
            seconds_%=86400;
            hours_=hours(seconds_/std::chrono::seconds(3600));
            seconds_%=3600;
            minutes_=minutes(seconds_/std::chrono::seconds(60));
            seconds_%=60;
        }
    }
    if(minutes_>minutes(0)){
        if(minutes_>=std::chrono::hours(1)){
            days_=days(minutes_/days(1));
            minutes_%=days(1);
            hours_=hours(minutes_/hours(1));
            minutes_%=hours(1);
        }
    }
    if(hours_>hours(0)){
        if(hours_>=std::chrono::days(1)){
            days_=days(hours_/days(1));
            hours_%=days(1);
        }
    }
}
TimeOffset::TimeOffset(const TimeOffset& other) noexcept{
    if(this!=&other){
        years_ = other.years_;
        months_ = other.months_;
        days_ = other.days_;
        hours_ = other.hours_;
        minutes_ = other.minutes_;
        seconds_ = other.seconds_;
    }
}
TimeOffset::TimeOffset(TimeOffset&& other) noexcept{
    if(this!=&other){
        years_ = std::move(other.years_);
        months_ = std::move(other.months_);
        days_ = std::move(other.days_);
        hours_ = std::move(other.hours_);
        minutes_ = std::move(other.minutes_);
        seconds_ = std::move(other.seconds_);
    }
}
TimeOffset& TimeOffset::operator=(const TimeOffset& other) noexcept{
    if(this!=&other){
        years_ = other.years_;
        months_ = other.months_;
        days_ = other.days_;
        hours_ = other.hours_;
        minutes_ = other.minutes_;
        seconds_ = other.seconds_;
    }
    return *this;
}
TimeOffset& TimeOffset::operator=(TimeOffset&& other) noexcept{
    if(this!=&other){
        years_ = std::move(other.years_);
        months_ = std::move(other.months_);
        days_ = std::move(other.days_);
        hours_ = std::move(other.hours_);
        minutes_ = std::move(other.minutes_);
        seconds_ = std::move(other.seconds_);
    }
    return *this;
}

/**
 * @return Next time-point from current time-point by set time-offset
 */
utc_tp TimeOffset::get_next_tp(utc_tp current_time) const noexcept{
    using namespace std::chrono;
    auto ymd = year_month_day(floor<days>(current_time));
    ymd+=years_;
    ymd+=months_;
    auto sd = utc_tp(sys_days(ymd));
    sd+=days_+hours_;
    return sd;
}
/**
 * @return The time-point aligned to the lesser defined time-unit
 */
utc_tp TimeOffset::get_null_aligned_tp(utc_tp current_time, utc_tp from_initial_interval) const noexcept{
    using namespace std::chrono;
    auto ymd_current = year_month_day(floor<days>(current_time));
    auto ymd_initial = year_month_day(floor<days>(from_initial_interval));
    years year;
    if(seconds_>std::chrono::seconds(0))

    if(years_!=years(0)){
        year = ((ymd_current.year()-ymd_initial.year())/years_)*years_;
        if(year==years(0))
            return from_initial_interval;
    }
    months month;
    if(months_!=months(0)){
        if(months_>months(11)){
            year=years_+years(months_/months(12));
            month=months(months_%months(12));
        }
        if(years_!=years(0)){
            year = ((ymd_current.year()-ymd_initial.year())/year)*year;
            if(year==years(0))
                return from_initial_interval;
        }
        month=(ymd_current.month()-ymd_initial.month())/month*month;
        if(month==months(0))
            return from_initial_interval;
    }
    days day;
    if(days_>days(0)){
        day=(sys_days(floor<days>(current_time))-sys_days(ymd_initial+year+month))/days_*days_;
        if(day==days(0))
            return from_initial_interval;
    }
    utc_tp result = sys_days(ymd_initial+year+month)+day;
    hours hour;
    if(hours_>hours(0)){
        hour=(current_time-result)/hours_*hours_;
        if(hour==hours(0))
            return from_initial_interval;
        else result+=hour;
    }
    minutes minute;
    if(minutes_>minutes(0)){
        minute=(current_time-result)/minutes_*minutes_;
        if(minute==minutes(0))
            return from_initial_interval;
        else result+=minute;
    }
    std::chrono::seconds second;
    if(seconds_>std::chrono::seconds(0)){
        second=(current_time-result)/seconds_*seconds_;
        if(second==std::chrono::seconds(0))
            return from_initial_interval;
        else result+=second;
    }
    return result;
}

template<>
boost::json::value to_json(const TimeOffset& val){
    boost::json::object result;
    result["years"] = val.years_.count();
    result["months"] = val.months_.count();
    result["days"] = val.days_.count();
    result["hours"] = val.hours_.count();
    result["minutes"] = val.minutes_.count();
    result["seconds"] = val.seconds_.count();
    return result;
}

template<>
std::expected<TimeOffset,std::exception> from_json(const boost::json::value& val){
    TimeOffset result;
    try{
        result.years_=years(val.at("years").as_int64());
        result.months_=months(val.at("months").as_int64());
        result.days_=days(val.at("days").as_int64());
        result.hours_=hours(val.at("hours").as_int64());
        result.minutes_=minutes(val.at("minutes").as_int64());
        result.seconds_=std::chrono::seconds(val.at("seconds").as_int64());
    }
    catch(const std::invalid_argument& err){
        return std::unexpected(err);
    }
    return result;
}

#include "parsing.h"
#include <iostream>

template<>
TimeOffset boost::lexical_cast(const std::string& input){
    using namespace std::string_literals;
    TimeOffset result;
    std::vector<std::string_view> tokens = split<std::string_view>(std::string_view(input),":");
    if(!tokens.empty()){
        for(std::string_view token:tokens){
            auto tmp(from_chars<int>(token.substr(1)));
            if(!tmp.has_value())
                throw std::invalid_argument(input);
            else{
                if(tmp.value()<0){
                    throw std::invalid_argument("Invalid time offset token input "s+std::string(token));
                }
                else if(tmp.value()==0){
                    std::cout<<"Ignored value: "s<<token<<std::endl;
                    continue;
                }
            }
            if(token.size()>0){
                if(token.starts_with("h"))
                    result.hours_ = hours(tmp.value());
                else if(token.starts_with("y"))
                    result.years_ = years(tmp.value());
                else if(token.starts_with("m"))
                    result.months_ = months(tmp.value());
                else if(token.starts_with("d"))
                    result.days_ = days(tmp.value());
                else{
                    std::cout<<"Unknown time offset token"<<std::endl;
                    throw std::invalid_argument(input);
                }
            }
            else{
                std::cout<<"Missed time offset token"<<std::endl;
                throw std::invalid_argument(input);
            }
        }
        return result;
    }
    else{
        std::cout<<"Empty string at time offset definition"<<std::endl;
        throw std::invalid_argument(input);
    }
}

template<>
utc_tp boost::lexical_cast(const std::string& input){
    std::istringstream s(input);
    s.imbue(std::locale("en_US.utf-8"));
    utc_tp tp;
    s>>std::chrono::parse("{:%D-%T}",tp);
    if(s.fail())
        throw boost::bad_lexical_cast();
    return tp;
}

template<>
std::string boost::lexical_cast(const utc_tp& input){
    return std::format("{:%D-%T}",input);
}
template<>
std::string boost::lexical_cast(const TimeOffset& input){
    std::string result;
    if(input.years_.count()>0)
        result+="y"+std::to_string(input.years_.count());
    if(input.months_.count()>0)
        result+="m"+std::to_string(input.months_.count());
    if(input.days_.count()>0)
        result+="d"+std::to_string(input.days_.count());
    if(input.hours_.count()>0)
        result+="h"+std::to_string(input.hours_.count());    
    return result;
}