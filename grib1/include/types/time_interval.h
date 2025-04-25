#pragma once
#include <stdbool.h>
#include <chrono>
#include <unordered_map>
#include "aux_code/def.h"

using MinTimeRange = std::chrono::duration<int64_t, std::ratio<3600L>>;

using namespace std::chrono;
using utc_tp = system_clock::time_point;
using utc_diff = std::chrono::system_clock::duration;

struct TimeOffset{
    years years_ = years(0);
    months months_ = months(0);
    days days_ = days(0);
    hours hours_ = hours(0);
    minutes minutes_ = minutes(0);
    std::chrono::seconds seconds_ = std::chrono::seconds(0);
};

struct TimeInterval{
    utc_tp from_;
    utc_tp to_;
};

struct TimeSequence{
    TimeInterval interval_;
    utc_diff discret_;
};

template<>
struct std::hash<TimeInterval>{
    size_t operator()(const TimeInterval& val){
        return std::hash<size_t>{}(static_cast<size_t>(val.from_.time_since_epoch().count())^(static_cast<size_t>(val.from_.time_since_epoch().count()<<1)));
    }
};

template<>
struct std::equal_to<TimeInterval>{
    bool operator()(const TimeInterval& lhs,const TimeInterval& rhs) const{
        return lhs.from_==rhs.from_ && lhs.to_==rhs.to_;
    }
};

template<>
struct std::less<TimeInterval>{
    bool operator()(const TimeInterval& lhs,const TimeInterval& rhs) const{
        return (lhs.from_<rhs.from_)?true:(lhs.from_==rhs.from_?lhs.to_-lhs.from_<rhs.to_-rhs.from_:false);
    }
};

bool is_correct_interval(const utc_tp& from,const utc_tp& to);
bool intervals_intersect(const TimeInterval& lhs, const TimeInterval& rhs);
bool intervals_intersect(const utc_tp& from_1, const utc_tp& to_1,const utc_tp& from_2, const utc_tp& to_2);
std::pair<uint16_t,uint16_t> interval_intersection_pos(const TimeInterval& to_seek, const TimeInterval& initial, const utc_diff& discret) noexcept;