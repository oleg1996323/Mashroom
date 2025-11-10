#pragma once
#include <cinttypes>
#include <string>
#include <unordered_map>

struct IndexDataFileFormats{
    enum token:uint32_t{
        NATIVE,
        BINARY,
        JSON,
        XML, 
        IEEE, 
        GRIB,
        HGT,
        NETCDF
    };
};

std::string token_to_extension(IndexDataFileFormats::token token);
const std::unordered_map<IndexDataFileFormats::token,std::string>& token_extension() noexcept;

inline IndexDataFileFormats::token operator|(IndexDataFileFormats::token lhs,IndexDataFileFormats::token rhs){
    return static_cast<IndexDataFileFormats::token>(static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(lhs)|
                    static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(rhs));
}
inline IndexDataFileFormats::token operator&(IndexDataFileFormats::token lhs,IndexDataFileFormats::token rhs){
    return static_cast<IndexDataFileFormats::token>(static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(lhs)&
                        static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(rhs));
}
inline IndexDataFileFormats::token& operator|=(IndexDataFileFormats::token& lhs,IndexDataFileFormats::token rhs){
    lhs=lhs|rhs;
    return lhs;
}
inline IndexDataFileFormats::token& operator&=(IndexDataFileFormats::token& lhs,IndexDataFileFormats::token rhs){
    lhs=lhs&rhs;
    return lhs;
}
inline IndexDataFileFormats::token operator^(IndexDataFileFormats::token lhs,IndexDataFileFormats::token rhs){
    return static_cast<IndexDataFileFormats::token>(static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(lhs)^
                            static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(rhs));
}
inline IndexDataFileFormats::token operator~(IndexDataFileFormats::token val){
    return static_cast<IndexDataFileFormats::token>(~(static_cast<std::underlying_type_t<IndexDataFileFormats::token>>(val)));
}