#pragma once
#include "OsterLib/types/coord.h"

namespace projection::modificator{
    class AbstractStretch{
        public:
        AbstractStretch() = default;

    };
constexpr Lat ysp_default = 0.;
constexpr Lon xsp_default = 0.;
constexpr double s_factor_default = 1.;
class Stretch{
    /// @brief Latitude of pole of stretching in millidegrees (integer)
    Lat ysp = ysp_default;
    /// @brief Longitude of pole of stretching in millidegrees (integer)
    Lon xsp = xsp_default;
    /// @brief Stretching factor (representation as for the reference value) (ibmfloat)
    double s_factor = s_factor_default;
    public:
    bool operator==(const Stretch& other) const noexcept{
        return ysp==other.ysp && xsp==other.xsp && s_factor==other.s_factor;
    }
    void stretch_factor(double factor) noexcept{
        s_factor = factor;
    }
    /// @brief set longitude of pole of stretching in millidegrees (integer)
    void stretch_pole_longitude(Lon lon) noexcept{
        xsp = lon;
    }
    /// @brief set latitude of pole of stretching in millidegrees (integer)
    void stretch_pole_latitude(float lat) noexcept{
        ysp = lat;
    }
    /// @brief stretch factor
    double factor() const noexcept{
        return s_factor;
    }
    /// @brief return coordinates of pole of stretching in millidegrees (integer)
    virtual Coord pole() const noexcept{
        return Coord{.lat_=ysp,.lon_=xsp};
    }
    bool is_default() const noexcept{
        return (ysp==ysp_default &&
            s_factor==s_factor_default) ||
            (xsp==xsp_default &&
            s_factor==s_factor_default);
            
    }
};
}