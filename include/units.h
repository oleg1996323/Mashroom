#pragma once
#include <cinttypes>
#include <array>
#include <string_view>

struct UnitsEnum{
    enum type{
        InformationSize
    };
};

using Units_t = UnitsEnum::type;

template<UnitsEnum::type T>
struct Units;

template<>
struct Units<UnitsEnum::InformationSize>{
    enum type:uint8_t{
        BYTE,
        KILOBYTE,
        MEGABYTE,
        GIGABYTE,
        TERABYTE
    };
    static constexpr std::array<const char*,5> txt = {
        "B",
        "KB",
        "MB",
        "GB",
        "TB"
    };
};

namespace units{
    using Information = Units<Units_t::InformationSize>;
}

template<typename T>
inline T translate_from_txt(std::string_view cmd) noexcept{
    for(int i=0;i<Units<T>::txt.size();++i){
        if(cmd==Units<T>::txt[i])
            return (T)i;
    }
    return (T)-1;
}

template<typename T>
inline std::string_view translate_from_token(T token) noexcept{
    return Units<T>::txt.at((uint)token);
}