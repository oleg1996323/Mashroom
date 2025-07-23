#pragma once
#include <cinttypes>
#include <cstddef>
#include <type_traits>

enum class OutputDataFileFormats : uint32_t{
    DEFAULT = 0,
    TXT_F = 1,
    BIN_F = (1 << 1),
    GRIB_F = (1 << 2),
    ARCHIVED = (1 << 3)
};

inline OutputDataFileFormats operator|(OutputDataFileFormats lhs,OutputDataFileFormats rhs){
    return (OutputDataFileFormats)(static_cast<uint32_t>(lhs)|static_cast<uint32_t>(rhs));
}
inline OutputDataFileFormats operator&(OutputDataFileFormats lhs,OutputDataFileFormats rhs){
    return (OutputDataFileFormats)(static_cast<uint32_t>(lhs)&static_cast<uint32_t>(rhs));
}
inline OutputDataFileFormats& operator|=(OutputDataFileFormats& lhs,OutputDataFileFormats rhs){
    return lhs=lhs|rhs;
}
inline OutputDataFileFormats& operator&=(OutputDataFileFormats& lhs,OutputDataFileFormats rhs){
    return lhs=lhs&rhs;
}
inline OutputDataFileFormats operator^(OutputDataFileFormats lhs,OutputDataFileFormats rhs){
    return (OutputDataFileFormats)(static_cast<uint32_t>(lhs)^static_cast<uint32_t>(rhs));
}
inline OutputDataFileFormats operator~(OutputDataFileFormats val){
    return (OutputDataFileFormats)(~(static_cast<uint32_t>(val)));
}