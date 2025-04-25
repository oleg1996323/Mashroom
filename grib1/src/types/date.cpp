#include "types/time_interval.h"
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_1.h"

bool is_correct_interval(const std::chrono::system_clock::time_point& from,const std::chrono::system_clock::time_point& to){
    return (to-from).count()>0;
}
bool intervals_intersect(const TimeInterval& lhs, const TimeInterval& rhs){
    return !(lhs.from_>rhs.to_  || lhs.to_<rhs.from_);
}
bool intervals_intersect(const utc_tp& from_1, const utc_tp& to_1,const utc_tp& from_2, const utc_tp& to_2){
    return !(from_1>to_2  || to_1<from_2);
}
#include <iostream>
std::pair<uint16_t,uint16_t> interval_intersection_pos(const TimeInterval& to_seek, const TimeInterval& initial, const utc_diff& discret) noexcept{
    std::pair<uint16_t,uint16_t> result{0,0};
    if(discret.count()==0)
        return result;
    if(to_seek.from_<=initial.from_)
        result.first = 0;
    else
        result.first = (to_seek.from_.time_since_epoch()-initial.from_.time_since_epoch())/discret+1;

    if(to_seek.to_>=initial.to_)
        result.second = (initial.to_.time_since_epoch()-initial.from_.time_since_epoch())/discret+1;
    else{
        result.second = (initial.to_.time_since_epoch()-to_seek.to_.time_since_epoch())/discret+1;
    }
    //result.first = to_seek.from_<=initial.from_?0:(to_seek.from_-initial.from_)/discret;
    return result;
}