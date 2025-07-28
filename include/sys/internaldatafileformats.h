#pragma once
#include <cinttypes>

enum class InternalDateFileFormats:uint32_t{
    NATIVE,
    BINARY,
    TEXT, 
    IEEE, 
    GRIB,
    HGT,
    NETCDF
};

inline InternalDateFileFormats operator|(InternalDateFileFormats lhs,InternalDateFileFormats rhs){
    return (InternalDateFileFormats)(static_cast<uint32_t>(lhs)|static_cast<uint32_t>(rhs));
}
inline InternalDateFileFormats operator&(InternalDateFileFormats lhs,InternalDateFileFormats rhs){
    return (InternalDateFileFormats)(static_cast<uint32_t>(lhs)&static_cast<uint32_t>(rhs));
}
inline InternalDateFileFormats& operator|=(InternalDateFileFormats& lhs,InternalDateFileFormats rhs){
    return lhs=lhs|rhs;
}
inline InternalDateFileFormats& operator&=(InternalDateFileFormats& lhs,InternalDateFileFormats rhs){
    return lhs=lhs&rhs;
}
inline InternalDateFileFormats operator^(InternalDateFileFormats lhs,InternalDateFileFormats rhs){
    return (InternalDateFileFormats)(static_cast<uint32_t>(lhs)^static_cast<uint32_t>(rhs));
}
inline InternalDateFileFormats operator~(InternalDateFileFormats val){
    return (InternalDateFileFormats)(~(static_cast<uint32_t>(val)));
}