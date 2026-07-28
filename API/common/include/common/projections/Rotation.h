#pragma once
#include "OsterLib/types/coord.h"

namespace projection::modificator{

constexpr Lat yp_default = 0.;
constexpr Lon xp_default = 0.;
constexpr double ang_default = 1.;

class Rotation{
    /// @brief Latitude of the southern pole in millidegrees (integer)
    Lat yp = 0.;
    /// @brief Longitude of the southern pole in millidegrees (integer)
    Lon xp = 0.;
    /// @brief Angle of rotation (represented in the same way as the reference value) (ibmfloat)
    double ang = ang_default;
    public:
    bool operator==(const Rotation& other) const noexcept{
        return yp==other.yp && xp==other.xp && ang==other.ang;
    }

    /// @brief return longitude of the southern pole in millidegrees
    Coord southern_pole() const noexcept{
        return Coord{.lat_=yp,.lon_=xp};
    }
    /// @brief set longitude of the southern pole in millidegrees
    void southern_pole_longitude(Lon lon) noexcept{
        xp = lon;
    }
    /// @brief set latitude of the southern pole in millidegrees
    void southern_pole_latitude(Lat lat) noexcept{
        yp = lat;
    }
    /// @brief return angle of rotation (represented in the same way as the reference value) (ibmfloat)
    double rotation_angle() const noexcept{
        return ang;
    }
    /// @brief set angle of rotation (represented in the same way as the reference value) (ibmfloat)
    void rotation_angle(double angle) noexcept{
        ang=angle;
    }
    bool is_default() const noexcept{
        return yp==yp_default &&
            xp==xp_default &&
            ang==ang_default;
    }
};
}