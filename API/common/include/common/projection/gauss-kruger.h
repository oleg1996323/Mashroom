#pragma once
#include "AbstractProjection.h"
#include "proj/coordinateoperation.hpp"
#include "proj/crs.hpp"
#include "proj/io.hpp"
#include "proj/util.hpp"
#include "GeographicLib/TransverseMercator.hpp"
#include "GeographicLib/Ellipsoid.hpp"
#include "OsterLib/types/position.h"
#include "common/datum/AbstractDatum.h"

namespace projection{
    class GaussKruger final:public AbstractProjectionMaker<GaussKruger>{
        /// @brief zone number
        uint16_t zone_ = 0;
        /// @brief zone width (360 must be multiple of)
        double zone_width_ = 6;
        /// @brief Ложное восточное смещение (500000 м)
        double false_easting_ = 500000;
        double false_northing_ = 0;
        public:
        GaussKruger()=default;
        GaussKruger(const GaussKruger& other):
            AbstractProjectionMaker(
                other),
            zone_(other.zone_),
            zone_width_(other.zone_width_),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_){}
        GaussKruger(GaussKruger&& other):
            AbstractProjectionMaker(
                std::move(other)),
            zone_(other.zone_),
            zone_width_(other.zone_width_),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_){}
        GaussKruger& operator=(const GaussKruger& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(other);
                zone_ = other.zone_;
                zone_width_ = other.zone_width_;
                false_easting_= other.false_easting_;
                false_northing_= other.false_northing_;
            }
            return *this;
        }
        GaussKruger& operator=(GaussKruger&& other) noexcept{
            if(this!=&other){
                AbstractProjectionMaker::operator=(std::move(other));
                zone_ = other.zone_;
                zone_width_ = other.zone_width_;
                false_easting_= other.false_easting_;
                false_northing_= other.false_northing_;
            }
            return *this;
        }
        bool operator==(const GaussKruger& other) const noexcept{
            return AbstractProjectionMaker::operator==(other)&&
            (zone_==other.zone_)&&
            (zone_width_==other.zone_width_)&&
            (false_easting_==other.false_easting_)&&
            (false_northing_==other.false_northing_);
        }
        std::error_code zone(uint16_t z) noexcept{
            if (z > 0) {
                zone_ = z;
                return {};
            }
            return std::make_error_code(std::errc::invalid_argument);
        }

        uint16_t zone() const noexcept{
            return zone_;
        }

        std::error_code zone_width(double w) noexcept{
            if (w > 0.0) {
                zone_width_ = w;
                return {};
            }
            return std::make_error_code(std::errc::invalid_argument);
        }

        double zone_width() const noexcept{
            return zone_width_;
        }

        void false_easting(double fe) noexcept{
            false_easting_ = fe;
        }

        void false_northing(double fn) noexcept{
            false_northing_ = fn;
        }

        double false_easting() const noexcept{
            return false_easting_;
        }

        double false_northing() const noexcept{
            return false_northing_;
        }

        /** @todo add a/f definition by datum
         * 
         */

        /** @brief zone_width_>0. and
                zone_>0 and
                zone_width_*zone_<=360
            @return 
        */
        virtual bool valid() const noexcept override{
            return projection().empty() &&
                zone_width_>0.+std::numeric_limits<double>::epsilon() &&
                zone_>0 &&
                zone_width_*zone_<=360+std::numeric_limits<double>::epsilon() && datum_;
        }

        bool has_datum() const noexcept{
            return datum_.get()!=nullptr;
        }

        bool southern() const noexcept{
            return false_northing_>=10000000;
        }

        std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept{
            std::vector<Position> result;
            result.reserve(geodesic.size());
            GeographicLib::TransverseMercator transfomer(datum_->a(),datum_->f(),1);
            GeographicLib::Math::real lon0 = 
                GeographicLib::Math::real(zone_*zone_width_-zone_width_/2)*GeographicLib::Math::degree();
            for(auto& [lat,lon]:geodesic){
                GeographicLib::Math::real x;
                GeographicLib::Math::real y;
                transfomer.Forward(lon0,
                    lat*GeographicLib::Math::degree(),
                    lon*GeographicLib::Math::degree(),x,y);
                result.push_back(Position{
                        .y_=y+false_northing(),
                        .x_=x+false_easting()});
            }
            return result;
        }

        std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept{
            std::vector<Coord> result;
            result.reserve(xy.size());
            GeographicLib::TransverseMercator transfomer(datum_->a(),datum_->f(),1);
            GeographicLib::Math::real lon0 = 
                GeographicLib::Math::real(zone_*zone_width_-zone_width_/2)*GeographicLib::Math::degree();
            for(auto& [northing,easting]:xy){
                double fn = (northing >= 10'000'000.0) ? 10'000'000.0 : 0.0;
                double x = easting - false_easting();
                double y = northing - fn;
                GeographicLib::Math::real lon_rad;
                GeographicLib::Math::real lat_rad;
                transfomer.Reverse(lon0,x,y,lat_rad,lon_rad);
                result.push_back(Coord{
                    .lat_=lat_rad/GeographicLib::Math::degree(),
                    .lon_=lon_rad/GeographicLib::Math::degree()});
            }
            return result;
        }

        virtual double left_bound([[maybe_unused]] Lat latitude) const noexcept override{
            return zone_width_*zone_-zone_width_/2;
        }
        virtual double right_bound([[maybe_unused]] Lat latitude) const noexcept override{
            return zone_width_*zone_+zone_width_/2;
        }
        virtual double top_bound([[maybe_unused]] Lon longitude) const noexcept override{
            return 90.;
        }
        virtual double bottom_bound([[maybe_unused]] Lon longitude) const noexcept override{
            return -90.;
        }
        virtual bool position_in(Coord position) const noexcept override{
            return position.lon_>=left_bound(position.lat_) &&
                position.lon_<=right_bound(position.lat_) &&
                position.lat_<=top_bound(position.lon_) &&
                position.lat_>=bottom_bound(position.lon_);
        }

        // Клонирование для полиморфного копирования
        virtual std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}