#pragma once
#include "Rotation.h"
#include "Stretch.h"
#include "common/projection_area/ProjectionArea.h"

namespace grid{

    class AbstractGrid{
        std::unique_ptr<projection::AbstractProjection> proj_; // maybe ProjectionArea???
        modificator::Rotation rotation_;
        modificator::Stretch stretch_;
        public:
        AbstractGrid()=default;
        AbstractGrid(const AbstractGrid& other):
            proj_(other.proj_?
                other.proj_->clone():
                std::unique_ptr<projection::AbstractProjection>()),
            rotation_(other.rotation_),
            stretch_(other.stretch_){}
        AbstractGrid(AbstractGrid&& other):
            rotation_(std::move(other.rotation_)),
            stretch_(std::move(other.stretch_)){}
        // Параметры проекции (могут быть опциональными)
        bool hasStretch() const noexcept{
            return !stretch_.is_default();
        }
        double stretchFactor() const noexcept{
            return stretch_.factor();
        }
        Coord stretchPole() const noexcept{
            return stretch_.pole();
        }
        bool hasRotation() const noexcept{
            return rotation_.is_default();
        }
        double rotationAngle() const noexcept{
            return rotation_.rotation_angle();
        }
        Coord southPole() const noexcept{
            return rotation_.southern_pole();
        }
        /// @brief set angle of rotation (represented in the same way as the reference value) (ibmfloat)
        void rotation_angle(double angle) noexcept{
            rotation_.rotation_angle(angle);
        }
        /// @brief set longitude of the southern pole in millidegrees
        void southern_pole_longitude(Lon lon) noexcept{
            rotation_.southern_pole_longitude(lon);
        }
        /// @brief set latitude of the southern pole in millidegrees
        void southern_pole_latitude(Lat lat) noexcept{
            rotation_.southern_pole_latitude(lat);
        }
        void stretch_factor(double factor) noexcept{
            stretch_.stretch_factor(factor);
        }
        /// @brief set longitude of pole of stretching in millidegrees (integer)
        void stretch_pole_longitude(Lon lon) noexcept{
            stretch_.stretch_pole_longitude(lon);
        }
        /// @brief set latitude of pole of stretching in millidegrees (integer)
        void stretch_pole_latitude(float lat) noexcept{
            stretch_.stretch_pole_latitude(lat);
        }
        bool set_projection(std::string_view name, std::string_view datum) noexcept{
            
        }
    };
}