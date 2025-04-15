#pragma once
#include "aux_code/def.h"
#include "unordered_map"
#include "code_tables/table_4.h"
#include "code_tables/table_0.h"

STRUCT_BEG(CommonDataProperties)
{
    std::optional<TimeFrame> fcst_unit_ ={};
    Organization center_ = WMO;
    uint8_t subcenter_ = 0;
    uint8_t parameter_ = 0;

    CommonDataProperties(Organization center,uint8_t subcenter,TimeFrame fcst_unit,uint8_t parameter):
    center_(center),subcenter_(subcenter),fcst_unit_(fcst_unit),parameter_(parameter){}
    CommonDataProperties() = default;
    bool operator==(const CommonDataProperties& other) const{
        return center_==other.center_ && subcenter_==other.subcenter_ && fcst_unit_==other.fcst_unit_ && parameter_==other.parameter_;
    }
}
STRUCT_END(CommonDataProperties)

template<>
struct std::hash<CommonDataProperties>{
    size_t operator()(const CommonDataProperties& cs) const{
        return std::hash<size_t>{}(static_cast<size_t>(cs.center_)<<24+static_cast<size_t>(cs.subcenter_)<<16+
        static_cast<size_t>(cs.fcst_unit_.has_value()?cs.fcst_unit_.value():0)<<8+cs.parameter_);
    }
};