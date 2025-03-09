#pragma once
#include <cinttypes>

enum class Collection : uint8_t{
    NONE,
    ERA5_RECT,  //ERA5 прямоугольная СК
    ERA5_GAUSS, //ERA5 Гаусс СК
    MERRA2,     //Merra2
    SRTM1,      //SRTM 1 arc
    SRTM3,      //SRTM 3 arc
    ROUGH,      //Шероховатость
    RGMC        //РосГидроМетЦентр
};
enum class Tag:uint16_t{
    None,
    T2,
    SP,
    V100,
    U100,
    V10,
    U10,
    EWSS,
    I10FG,
    IEWS,
    INSS
    //may be continued
};