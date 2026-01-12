#pragma once
#include "types/time_period.h"
#include <variant>
#include <string>
#include "concepts.h"
#include "types/time_interval.h"
#include <unordered_map>
#include <cstdint>
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include <cassert>

class TimeForecast{
    public:
    struct period_t{
        uint8_t val ;
    };
    protected:
    uint16_t N_ = -1;
    uint8_t N_avg_acc_missed_ = 0;
    period_t P1_ = {0};
    period_t P2_ = {0};
    TimeFrame tf_;
    TimeRangeIndicator tri_;

    private:
    const char* __time_frame_to_fmt__() const;
    friend struct std::hash<TimeForecast>;
    template<bool,auto>
    friend struct serialization::Serialize;
    template<bool,auto>
    friend struct serialization::Deserialize;
    template<auto>
    friend struct serialization::Serial_size;
    template<auto>
    friend struct serialization::Min_serial_size;
    template<auto>
    friend struct serialization::Max_serial_size;
    template<typename T>
    friend std::expected<T,std::exception> from_json(const boost::json::value& val);
    public:
    TimeForecast() = default;
    TimeForecast(TimeFrame tf, TimeRangeIndicator tri, period_t P1, period_t P2):
    tf_(tf),tri_(tri),P1_(P1),P2_(P2){}

    TimeForecast(const TimeForecast& other) noexcept{
        *this = other;
    }
    TimeForecast(TimeForecast&& other) noexcept{
        *this = std::move(other);
    }
    uint16_t get_n() const{
        return N_;
    }
    uint8_t get_avg_acc_miss_N_vals() const{
        return N_avg_acc_missed_;
    }
    period_t get_P1() const{
        return P1_;
    }
    period_t get_P2() const{
        return P2_;
    }
    TimeFrame get_time_frame() const{
        return tf_;
    }
    void set_N(uint16_t N) noexcept{
        N_ = N;
    }
    void set_N_missed(uint8_t N_missed) noexcept{
        N_avg_acc_missed_ = N_missed;
    }
    void set_P1(period_t P1) noexcept{
        P1_ = P1;
    }
    void set_P2(period_t P2) noexcept{
        P2_ = P2;
    }
    void set_time_frame(TimeFrame tf) noexcept{
        tf_ = tf;
    }
    void set_time_range_indicator(TimeRangeIndicator tri) noexcept{
        tri_ = tri;
    }
    TimeRangeIndicator get_time_range_indicator() const{
        return tri_;
    }
    bool operator==(const TimeForecast& other) const{
        return  N_==other.N_ && N_avg_acc_missed_==other.N_avg_acc_missed_ &&
                P1_.val==other.P1_.val && P2_.val == other.P2_.val &&
                tf_ == other.tf_ && tri_ == other.tri_;
    }
    bool operator!=(const TimeForecast& other) const noexcept{
        return !(*this==other);
    }
    TimeForecast& operator=(const TimeForecast& other) noexcept{
        if(&other!=this){
            N_=other.N_;
            N_avg_acc_missed_ = other.N_avg_acc_missed_;
            P1_.val = other.P1_.val;
            P2_.val = other.P2_.val;
            tf_ = other.tf_;
            tri_ = other.tri_;
        }
        return *this;
    }
    TimeForecast& operator=(TimeForecast&& other){
        if(&other!=this){
            N_=other.N_;
            N_avg_acc_missed_ = other.N_avg_acc_missed_;
            P1_.val = other.P1_.val;
            P2_.val = other.P2_.val;
            tf_ = other.tf_;
            tri_ = other.tri_;
        }
        return *this;
    }

    // template<IsDuration DUR>
    // std::string time_stamp_to_string(utc_tp_t<DUR> initial, const std::string& time_format = "%Y:%m:d %H:%M:%S"){
    //     std::string ts_result = std::vformat("{:"+time_format+"}",std::make_format_args(initial));
    //     if(is_range()){
    //         switch (tri_)
    //         {
    //             case TimeRangeIndicator::AVG_RANGE_M_P1_M_P2:
    //                 return "avg{P1=-"s+__time_frame_to_fmt__()+"%d:P2=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::AVG_RANGE_M_P1_P_P2:
    //                 return "avg{P1=-"s+__time_frame_to_fmt__()+":P2=+"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::RANGE_REF_TIME:
    //                 return "val{P1=-"s+__time_frame_to_fmt__()+":P2=+"s+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::AVG_RANGE_REF_TIME:
    //                 return "avg{P1=-"s+__time_frame_to_fmt__()+":P2=+"s+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::DIFF_RANGE_REF_TIME:
    //                 return "diff{P1=-"s+__time_frame_to_fmt__()+":P2=+"s+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::ACCUM_RANGE_REF_TIME:
    //                 return "sum{P1=-"s+__time_frame_to_fmt__()+":P2=+"s+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             default:
    //                 assert(false);
    //                 break;
    //         }
    //     }
    //     else if(is_intervaled()){
    //         switch (tri_){
    //             case TimeRangeIndicator::AVG_N_UNINIT_INTERVAL_P2:
    //                 return "avg{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::ACCUM_N_UNINIT_INTERVAL_P2:
    //                 return "avg{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::STD_DEV_FIRST_PERIOD_P1_INTERVAL_P2:
    //                 return "stddev{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::STD_DEV_N_FORECAST_PERIOD_P1_INTERVAL_P2:
    //                 return "stddev{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
    //                 return "avg{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::ACCUM_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
    //                 return "avg{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_REDUCED_P2:
    //                 return "avg{P1=+"s+__time_frame_to_fmt__()+"%d:dT=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::UNINIT_REF_TIME:
    //                 return "val{P1=+"s+__time_frame_to_fmt__()+"%d:P2=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //             case TimeRangeIndicator::P1_DOUBLE_OCTETS:
    //                 return "val{P1=+"s+__time_frame_to_fmt__()+"%d:P2=-"+__time_frame_to_fmt__()+":N=%d:missed=%d}";
    //                 break;
    //         }
    //     }
    // }

    bool is_range() const;
    bool is_intervaled() const;
    bool octet_doubled() const{
        return tri_==TimeRangeIndicator::P1_DOUBLE_OCTETS;
    }
    bool missed() const{
        return tri_==TimeRangeIndicator::MISSING;
    }
    bool is_forecast() const;
    bool is_analysis() const;
    bool is_unique_value() const{
        return tri_==TimeRangeIndicator::UNINIT_REF_TIME ||
        tri_==TimeRangeIndicator::INIT_REF_TIME ||
        tri_==TimeRangeIndicator::P1_DOUBLE_OCTETS;
    }
};

TimeForecast time_forecast_from_string(const std::string& time_period);

template<>
struct std::hash<TimeForecast>{
    size_t operator()(const TimeForecast& val) const;
};

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,TimeForecast>{
        using type = TimeForecast;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.N_,msg.N_avg_acc_missed_,msg.P1_.val,msg.P2_.val,msg.tf_,msg.tri_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,TimeForecast>{
        using type = TimeForecast;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.N_,msg.N_avg_acc_missed_,msg.P1_.val,msg.P2_.val,msg.tf_,msg.tri_);
        }
    };

    template<>
    struct Serial_size<TimeForecast>{
        using type = TimeForecast;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.N_,msg.N_avg_acc_missed_,msg.P1_.val,msg.P2_.val,msg.tf_,msg.tri_);
        }
    };

    template<>
    struct Min_serial_size<TimeForecast>{
        using type = TimeForecast;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::N_),decltype(type::N_avg_acc_missed_),decltype(decltype(type::P1_)::val),decltype(decltype(type::P2_)::val),
            decltype(type::tf_),decltype(type::tri_)>();
        }();
    };

    template<>
    struct Max_serial_size<TimeForecast>{
        using type = TimeForecast;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::N_),decltype(type::N_avg_acc_missed_),decltype(decltype(type::P1_)::val),decltype(decltype(type::P2_)::val),
            decltype(type::tf_),decltype(type::tri_)>();
        }();
    };
}

#include "boost_functional/json.h"

template<>
std::expected<TimeForecast,std::exception> from_json<TimeForecast>(const boost::json::value& val);

template<>
boost::json::value to_json(const TimeForecast& val);