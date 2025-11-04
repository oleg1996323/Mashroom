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
inline bool operator==(int lhs,OutputDataFileFormats rhs) noexcept{
    return lhs==static_cast<std::underlying_type_t<OutputDataFileFormats>>(rhs);
}
inline bool operator==(OutputDataFileFormats lhs,int rhs) noexcept{
    return rhs==lhs;
}
inline bool operator!=(int lhs,OutputDataFileFormats rhs) noexcept{
    return !(lhs==rhs);
}
inline bool operator!=(OutputDataFileFormats lhs,int rhs) noexcept{
    return rhs!=lhs;
}