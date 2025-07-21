#pragma once
#include <stdbool.h>
#include <chrono>
#include <unordered_map>
#include "library/include/def.h"
#include "library/include/serialization.h"

using MinTimeRange = std::chrono::duration<int64_t, std::ratio<3600L>>;

using namespace std::chrono;
using utc_tp = system_clock::time_point;
using utc_diff = std::chrono::system_clock::duration;

#include <boost/program_options.hpp>

struct TimeOffset{
    years years_ = years(0);
    months months_ = months(0);
    days days_ = days(0);
    hours hours_ = hours(0);
    minutes minutes_ = minutes(0);
    std::chrono::seconds seconds_ = std::chrono::seconds(0);
    template<bool NETWORK_ORDER>
    friend struct serialization::Serialize;
    template<bool NETWORK_ORDER>
    friend struct serialization::Deserialize;
    friend struct serialization::Serial_size<TimeOffset>;
    friend struct serialization::Max_serial_size<TimeOffset>;
    friend struct serialization::Min_serial_size<TimeOffset>;
    public:
    TimeOffset() noexcept = default;
    explicit TimeOffset(years y,months mo,days d,hours h,minutes m,std::chrono::seconds s) noexcept;
    TimeOffset(const TimeOffset& other) noexcept;
    TimeOffset(TimeOffset&& other) noexcept;
    TimeOffset& operator=(const TimeOffset& other) noexcept;
    TimeOffset& operator=(TimeOffset&& other) noexcept;

    /**
     * @return Next time-point from current time-point by set time-offset
     */
    utc_tp get_next_tp(utc_tp current_time) const noexcept;
    /**
     * @return The time-point aligned to the lesser defined time-unit
     */
    utc_tp get_null_aligned_tp(utc_tp current_time, utc_tp from_initial_interval) const noexcept;

    inline bool is_set() const noexcept{
        return years_!=years(0) || months_!=months(0) || days_!=days(0) ||
                hours_!=hours(0) || minutes_!=minutes(0) || seconds_!=std::chrono::seconds(0);
    }
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

namespace serialization{

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,TimeOffset>{
        auto operator()(const TimeOffset& val,std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.years_,val.months_,val.days_,val.hours_,val.minutes_,val.seconds_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,TimeOffset>{
        auto operator()(TimeOffset& val,std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.years_,val.months_,val.days_,val.hours_,val.minutes_,val.seconds_);
        }
    };

    template<>
    struct Serial_size<TimeOffset>{
        size_t operator()(const TimeOffset& val) noexcept{
            return serial_size(val.years_,val.months_,val.days_,val.hours_,val.minutes_,val.seconds_);
        }
    };

    template<>
    struct Min_serial_size<TimeOffset>{
        constexpr size_t operator()(const TimeOffset& val) noexcept{
            return min_serial_size(val.years_,val.months_,val.days_,val.hours_,val.minutes_,val.seconds_);
        }
    };
     
    template<>
    struct Max_serial_size<TimeOffset>{
        constexpr size_t operator()(const TimeOffset& val) noexcept{
            return max_serial_size(val.years_,val.months_,val.days_,val.hours_,val.minutes_,val.seconds_);
        }
    };
}

#include "boost_functional/json.h"
#include <expected>

template<>
boost::json::value to_json(const TimeOffset& val);

template<>
std::expected<TimeOffset,std::exception> from_json(const boost::json::value& val);