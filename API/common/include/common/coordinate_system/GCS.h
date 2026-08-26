#pragma once
#include "common/datum/AbstractDatum.h"
#include "OsterLib/types/coord.h"

namespace coordinate_system{
    enum class units_t:uint8_t{
        GRAD,
        RAD,
        DEG
    };
    class GCS{
        Lon zero_lon_relGw_ = 0.; //начальная долгота относительно Гринвича
        units_t units_ = units_t::DEG; //единицы измерения
        bool s2n_dir_ = false; //направление с юга на север
        bool e2w_dir_ = false; //направление с востока на запад
        public:
        GCS()=default;
        void units(units_t u) noexcept{
            units_ = u;
        }
        units_t units() const noexcept{
            return units_;
        }
        void south2north(bool s2n = true) noexcept{
            s2n_dir_ = s2n;
        }
        bool south2north() const noexcept{
            return s2n_dir_;
        }
        void east2west(bool e2w = true) noexcept{
            e2w_dir_ = e2w;
        }
        bool east2west() const noexcept{
            return e2w_dir_;
        }
        //relative to Greenwich
        void zero_longitude(Lon longitude = 0) noexcept{
            zero_lon_relGw_ = longitude;
        }
        Lon zero_longitude() const noexcept{
            return zero_lon_relGw_;
        }
    };
}