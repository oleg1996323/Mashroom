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

    // template<bool NETWORK_ORDER = true>
    // void serialize(const TimeOffset& off, std::vector<char>& buf) noexcept{
    //     auto years_int = off.years_.count();
    //     auto months_int = off.months_.count();
    //     auto days_int = off.days_.count();
    //     auto hours_int = off.hours_.count();
    //     auto minutes_int = off.minutes_.count();
    //     auto seconds_int = off.seconds_.count();
    //     if constexpr(NETWORK_ORDER && is_little_endian()){
    //         years_int = std::byteswap(years_int);
    //         months_int = std::byteswap(months_int);
    //         days_int = std::byteswap(days_int);
    //         hours_int = std::byteswap(hours_int);
    //         minutes_int = std::byteswap(minutes_int);
    //         seconds_int = std::byteswap(seconds_int);
    //     }
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(years_int),reinterpret_cast<const char*>(years_int)+sizeof(years_int));
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(months_int),reinterpret_cast<const char*>(months_int)+sizeof(months_int));
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(days_int),reinterpret_cast<const char*>(days_int)+sizeof(days_int));
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(hours_int),reinterpret_cast<const char*>(hours_int)+sizeof(hours_int));
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(minutes_int),reinterpret_cast<const char*>(minutes_int)+sizeof(minutes_int));
    //     buf.insert(buf.end(),reinterpret_cast<const char*>(seconds_int),reinterpret_cast<const char*>(seconds_int)+sizeof(seconds_int));
    // }

    // template<>
    // constexpr size_t serial_size(const TimeOffset& val) noexcept{
    //     return sizeof(TimeOffset::years_.count())+sizeof(TimeOffset::months_.count())+sizeof(TimeOffset::days_.count())+
    //             sizeof(TimeOffset::hours_.count())+sizeof(TimeOffset::minutes_.count())+sizeof(TimeOffset::seconds_.count());
    // }
    // template<>
    // constexpr size_t min_serial_size(const TimeOffset& val) noexcept{
    //     return serial_size(val);
    // }
    // template<>
    // constexpr size_t max_serial_size(const TimeOffset& val) noexcept{
    //     return serial_size(val);
    // }
}