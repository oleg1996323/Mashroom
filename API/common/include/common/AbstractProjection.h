#pragma once
#include <utility>
#include "OsterLib/types/coord.h"
#include <string>
#include <optional>
#include "projections/definitions.h"

#include "projections/Rotation.h"
#include "projections/Stretch.h"


class AbstractProjection {
protected:
    std::string_view name_;
    projection::modificator::Rotation rotation_;
    projection::modificator::Stretch stretch_;
    template<typename PROJ>
    friend std::unique_ptr<
        AbstractProjection> make_projection(std::string_view name);
    AbstractProjection(std::string_view name):
    name_(std::move(name)){}
    friend std::unique_ptr<AbstractProjection> 
        make_projection(std::string_view projection) noexcept;
public:
    virtual ~AbstractProjection() = default;
    AbstractProjection(const AbstractProjection& other):
    name_(other.name_),
    rotation_(other.rotation_),
    stretch_(other.stretch_){}
    AbstractProjection(AbstractProjection&& other):
    name_(std::move(other.name_)),
    rotation_(std::move(other.rotation_)),
    stretch_(std::move(other.stretch_)){}
    AbstractProjection& operator=(const AbstractProjection& other){
        if(this!=&other){
            name_=other.name_;
            rotation_=other.rotation_;
            stretch_=other.stretch_;
        }
        return *this;
    }
    AbstractProjection& operator=(AbstractProjection&& other) noexcept{
        if(this!=&other){
            name_ = std::move(other.name_);
            rotation_ = std::move(other.rotation_);
            stretch_ = std::move(other.stretch_);
        }
        return *this;
    }
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
        return rotation_.rotation_angle();
    }
    double rotationAngle() const noexcept{
        return rotation_.is_default();
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

    // Клонирование для полиморфного копирования
    virtual std::unique_ptr<AbstractProjection> clone() const noexcept= 0;
};