#include "sections/product/time_forecast.h"

const char* TimeForecast::__time_frame_to_fmt__() const{
    switch (tf_)
        {
        case TimeFrame::NORMAL:
            return "%d 30Y";
            break;
        case TimeFrame::CENTURY:
            return "%d 100Y";
            break;
        case TimeFrame::DAY:
            return "%d D";
            break;
        case TimeFrame::DECADE:
            return "%d 10Y";
            break;
        case TimeFrame::HALF_HOUR:
            return "%d 30min";
            break;
        case TimeFrame::HOUR:
            return "%d h";
            break;
        case TimeFrame::HOURS_12:
            return "%d 12h";
            break;
        case TimeFrame::HOURS_3:
            return "%d 3h";
            break;
        case TimeFrame::HOURS_6:
            return "%d 6h";
            break;
        case TimeFrame::MINUTE:
            return "%d min";
            break;
        case TimeFrame::MONTH:
            return "%d M";
            break;
        case TimeFrame::QUARTER_HOUR:
            return "%d 15min";
            break;
        case TimeFrame::SECOND:
            return "%d s";
            break;
        case TimeFrame::YEAR:
            return "%d Y";
            break;
        case TimeFrame::INDIFFERENT:
            return "";
            break;
        default:
            assert(false);
            break;
        }
}

bool TimeForecast::is_forecast() const{
    switch(tri_){
        case TimeRangeIndicator::UNINIT_REF_TIME:{
            if(P1_.val>0)
                return true;
            else return false;
            break;
        }
        case TimeRangeIndicator::RANGE_REF_TIME:
        case TimeRangeIndicator::AVG_RANGE_M_P1_M_P2:
        case TimeRangeIndicator::AVG_RANGE_M_P1_P_P2:
        case TimeRangeIndicator::AVG_RANGE_REF_TIME:
        case TimeRangeIndicator::DIFF_RANGE_REF_TIME:
        case TimeRangeIndicator::ACCUM_RANGE_REF_TIME:
            return P2_.val>P1_.val;
            break;
        case TimeRangeIndicator::ACCUM_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::ACCUM_N_FORECAST_PERIOD_P1_REFS_P2:
        case TimeRangeIndicator::AVG_N_FORECAST_PERIOD_P1_REFS_P2:
        case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_REDUCED_P2:
        case TimeRangeIndicator::STD_DEV_N_FORECAST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::STD_DEV_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::TEMP_VAR_COVAR_N_REFS_AT_P2:
            return true;
            break;
        default: return false;
    }
}
bool TimeForecast::is_analysis() const{
    switch(tri_){
        case TimeRangeIndicator::UNINIT_REF_TIME:
        case TimeRangeIndicator::INIT_REF_TIME:
        case TimeRangeIndicator::AVG_N_FORECAST_PERIOD_P1_REFS_P2:
        case TimeRangeIndicator::ACCUM_N_FORECAST_PERIOD_P1_REFS_P2:
            if(P1_.val==0)
                return true;
            else return false;
        default:
            return false;
    }   
}
bool TimeForecast::is_range() const{
    switch(tri_){
        case TimeRangeIndicator::RANGE_REF_TIME:
        case TimeRangeIndicator::AVG_RANGE_M_P1_M_P2:
        case TimeRangeIndicator::AVG_RANGE_M_P1_P_P2:
        case TimeRangeIndicator::AVG_RANGE_REF_TIME:
        case TimeRangeIndicator::DIFF_RANGE_REF_TIME:
        case TimeRangeIndicator::ACCUM_RANGE_REF_TIME:
            return true;
        default:
            return false;
    }
}
bool TimeForecast::is_intervaled() const{
    switch(tri_){
        case TimeRangeIndicator::AVG_N_UNINIT_INTERVAL_P2:
        case TimeRangeIndicator::ACCUM_N_UNINIT_INTERVAL_P2:
        case TimeRangeIndicator::STD_DEV_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::STD_DEV_N_FORECAST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::ACCUM_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_P2:
        case TimeRangeIndicator::AVG_N_FORECAST_FIRST_PERIOD_P1_INTERVAL_REDUCED_P2:
            return true;
        default: return false;
    }
}
#include <boost/functional/hash.hpp>
size_t std::hash<TimeForecast>::operator()(const TimeForecast& val) const{
    size_t hash = 0;
    using namespace boost;
    hash_combine(hash,val.N_);
    hash_combine(hash,val.N_avg_acc_missed_);
    hash_combine(hash,val.P1_.val);
    hash_combine(hash,val.P2_.val);
    hash_combine(hash,static_cast<std::underlying_type_t<std::decay_t<decltype(val.tf_)>>>(val.tf_));
    hash_combine(hash,static_cast<std::underlying_type_t<std::decay_t<decltype(val.tri_)>>>(val.tri_));
    return hash;
}

template<>
std::expected<TimeForecast,std::exception> from_json<TimeForecast>(const boost::json::value& val){
    const static auto error = std::unexpected(std::invalid_argument("invalid json input"));
    if(val.is_object()){
        auto& obj = val.as_object();
        if(obj.contains("time frame") && obj.contains("time range indicator")){
            TimeForecast fcst;
            if(auto tf_tmp = from_json<TimeFrame>(obj.at("time frame"));!tf_tmp.has_value())
                return error;
            else 
                fcst.set_time_frame(tf_tmp.value());
            if(auto tri_tmp = from_json<TimeRangeIndicator>(obj.at("time range indicator"));!tri_tmp.has_value())
                return error;
            else 
                fcst.set_time_range_indicator(tri_tmp.value());
            if(fcst.is_intervaled()){
                if(obj.contains("N avg/acc") && obj.contains("N missed")){
                    if(auto N_avgacc_tmp = from_json<decltype(TimeForecast::N_)>(obj.at("N avg/acc"));!N_avgacc_tmp.has_value())
                        return error;
                    else 
                        fcst.set_N(N_avgacc_tmp.value());
                    if(auto N_missed_tmp = from_json<decltype(TimeForecast::N_avg_acc_missed_)>(obj.at("N missed"));!N_missed_tmp.has_value())
                        return error;
                    else 
                        fcst.set_N_missed(N_missed_tmp.value());
                }                
            }
            if(obj.contains("P1")){
                if(auto P1_tmp = from_json<decltype(TimeForecast::period_t::val)>(obj.at("P1"));!P1_tmp.has_value())
                    return error;
                else 
                    fcst.set_P1(TimeForecast::period_t{P1_tmp.value()});
            }
            else return error;
            if(!fcst.octet_doubled() && !fcst.is_unique_value()){
                if(obj.contains("P2")){
                    if(auto P2_tmp = from_json<decltype(TimeForecast::period_t::val)>(obj.at("P1"));!P2_tmp.has_value())
                        return error;
                    else 
                        fcst.set_P2(TimeForecast::period_t{P2_tmp.value()});
                    return fcst;
                }
                else return error;
            }
            return fcst;
        }
        else return error;
    }
    else return error;
}

template<>
boost::json::value to_json(const TimeForecast& val){
    boost::json::object result;
    result["time frame"] = to_json(val.get_time_frame());
    result["time range indicator"] = to_json(val.get_time_range_indicator());
    if(val.is_intervaled()){
        result["N avg/acc"] = to_json(val.get_n());
        result["N missed"] = to_json(val.get_avg_acc_miss_N_vals());
    }
    if(val.octet_doubled() && val.is_unique_value())
        result["P1"]=val.get_P1().val;
    else {
        result["P1"]=val.get_P1().val;
        result["P2"]=val.get_P2().val;
    }
    return result;
}