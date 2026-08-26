#pragma once
#include <utility>
#include "OsterLib/types/coord.h"
#include "OsterLib/types/position.h"
#include <string>
#include <optional>
#include "options.h"
#include "common/datum/AbstractDatum.h"
// #include "common/coordinate_system/GCS.h"

namespace projection{

class AbstractProjection {
protected:
    std::string_view name_;
    std::shared_ptr<datum::AbstractDatum> datum_ = datum::get_datum("wgs84");
    //coordinate_system::GCS gcs_; //geographic coordinate system
    template<typename PROJ>
    friend std::unique_ptr<
        AbstractProjection> make_projection(std::string_view name);
    friend std::unique_ptr<AbstractProjection> 
        make_projection(std::string_view projection) noexcept;
    void name(std::string_view name) noexcept{
        name_ = name;
    }
public:
    AbstractProjection() = default;
    virtual ~AbstractProjection() = default;
    AbstractProjection(const AbstractProjection& other):
    name_(other.name_),
    datum_(datum_){}
    AbstractProjection(AbstractProjection&& other):
    name_(std::move(other.name_)),
    datum_(std::move(datum_)){}
    AbstractProjection& operator=(const AbstractProjection& other){
        if(this!=&other){
            name_=other.name_;
            datum_ = std::move(datum_);
        }
        return *this;
    }
    AbstractProjection& operator=(AbstractProjection&& other) noexcept{
        if(this!=&other){
            name_ = std::move(other.name_);
            datum_ = std::move(datum_);
        }
        return *this;
    }
    bool operator==(const AbstractProjection& other) const noexcept{
        return name_ == other.name_;
    }
    bool datum(std::string_view datum_name) noexcept{
        auto d = datum::get_datum(datum_name);
        if(d){
            datum_ = d;
            return true;
        }
        else return false;        
    }
    std::string_view datum() const noexcept{
        return datum_->name();      
    }
    std::string_view projection() const noexcept{
        return name_;
    }
    virtual bool valid() const noexcept = 0;
    virtual double left_bound(Lat latitude) const noexcept = 0;
    virtual double right_bound(Lat latitude) const noexcept = 0;
    virtual double top_bound(Lon longitude) const noexcept = 0;
    virtual double bottom_bound(Lon longitude) const noexcept = 0;
    virtual bool position_in(Coord position) const noexcept = 0;
    virtual std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept = 0;
    virtual std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept = 0;
    static std::unique_ptr<AbstractProjection> make(std::string_view name) noexcept{
        return make_projection(name);
    }
    
    // Клонирование для полиморфного копирования
    virtual std::unique_ptr<AbstractProjection> clone() const noexcept= 0;
};

template<typename Projection_t>
class AbstractProjectionMaker:public AbstractProjection{
    public:
    static std::unique_ptr<Projection_t> make(std::string_view name) noexcept{
        static_assert(std::is_base_of_v<AbstractProjectionMaker<Projection_t>,Projection_t>);
        return make_projection<Projection_t>(name);
    }
};
}