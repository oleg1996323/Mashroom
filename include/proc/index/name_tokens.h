#pragma once
#include <cstdint>
#include <type_traits>

enum class DIR_TREE_TOKEN :uint64_t{
    HOUR = 1,
    DAY = 0b10,
    MONTH = 0b100,
    YEAR = 0b1000,
    LATITUDE = 0b10000,
    LONGITUDE = 0b100000,
    LATLON = LATITUDE|LONGITUDE,
    GRID = 0b10000000
};

inline DIR_TREE_TOKEN operator|(DIR_TREE_TOKEN lhs,DIR_TREE_TOKEN rhs){
    return static_cast<DIR_TREE_TOKEN>(static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(lhs)|
                    static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(rhs));
}
inline DIR_TREE_TOKEN operator&(DIR_TREE_TOKEN lhs,DIR_TREE_TOKEN rhs){
    return static_cast<DIR_TREE_TOKEN>(static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(lhs)&
                        static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(rhs));
}
inline DIR_TREE_TOKEN& operator|=(DIR_TREE_TOKEN& lhs,DIR_TREE_TOKEN rhs){
    lhs=lhs|rhs;
    return lhs;
}
inline DIR_TREE_TOKEN& operator&=(DIR_TREE_TOKEN& lhs,DIR_TREE_TOKEN rhs){
    lhs=lhs&rhs;
    return lhs;
}
inline DIR_TREE_TOKEN operator^(DIR_TREE_TOKEN lhs,DIR_TREE_TOKEN rhs){
    return static_cast<DIR_TREE_TOKEN>(static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(lhs)^
                            static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(rhs));
}
inline DIR_TREE_TOKEN operator~(DIR_TREE_TOKEN val){
    return static_cast<DIR_TREE_TOKEN>(~(static_cast<std::underlying_type_t<DIR_TREE_TOKEN>>(val)));
}