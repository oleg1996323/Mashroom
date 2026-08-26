#pragma once
#include "AbstractProjection.h"
#include "OsterLib/types/coord.h"
#include <utility>

namespace projection{
    class Spacial final:public AbstractProjectionMaker<Spacial>{
        /// @brief latitude of sub-satellite point
        Lat subsat_lat_;
        /// @brief longitude of sub-satellite point
        Lon subsat_lon_;
        /// @brief angle between Y-axis and projection vertical axis
        double orientation_;
        /// @brief height of satellite from Earth's center
        double Z_;
        public:
        Spacial(){
            datum("CGMS spacial"); //@todo handle if false
        }
        Spacial(const Spacial& other):
            AbstractProjectionMaker(
                other),
            subsat_lon_(other.subsat_lon_),
            subsat_lat_(other.subsat_lat_),
            orientation_(other.orientation_),
            Z_(other.Z_){}
        Spacial(Spacial&& other):
            AbstractProjectionMaker(
                std::move(other)),
            subsat_lon_(other.subsat_lon_),
            subsat_lat_(other.subsat_lat_),
            orientation_(other.orientation_),
            Z_(other.Z_){}
        Spacial& operator=(const Spacial& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(other);
                subsat_lon_ = other.subsat_lon_;
                subsat_lat_ = other.subsat_lat_;
                orientation_ = other.orientation_;
                Z_ = other.Z_;
            }
            return *this;
        }
        Spacial& operator=(Spacial&& other) noexcept{
            if(this!=&other){
                AbstractProjectionMaker::operator=(std::move(other));
                subsat_lon_ = other.subsat_lon_;
                subsat_lat_ = other.subsat_lat_;
                orientation_ = other.orientation_;
                Z_ = other.Z_;
            }
            return *this;
        }
        bool operator==(const Spacial& other) const noexcept{
            return AbstractProjectionMaker::operator==(other)&&
            (subsat_lon_==other.subsat_lon_)&&
            (subsat_lat_==other.subsat_lat_)&&
            (orientation_==other.orientation_)&&
            (Z_==other.Z_);
        }

        /// @brief longitude of sub-satellite point
        /// @brief долгота точки под спутником
        Lon subsat_lon() const noexcept{
            return subsat_lon_;
        }
        /// @brief latitude of sub-satellite point
        /// @brief долгота точки под спутником
        Lat subsat_lat() const noexcept{
            return subsat_lat_;
        }
        /// @brief projection's orientation
        /// @brief ориентация проекции
        double orientation() const noexcept{
            return orientation_;
        }
        /// @brief isometric height of satellite from Earth's center
        /// @brief изометрическая высота спутника от центра Земли
        double Z() const noexcept{
            return Z_;
        }
        /// @brief longitude of sub-satellite point
        /// @brief долгота точки под спутником
        Spacial& subsat_lon(Lon value) noexcept{
            subsat_lon_=value;
            return *this;
        }
        /// @brief latitude of sub-satellite point
        /// @brief долгота точки под спутником
        Spacial& subsat_lat(Lat value) noexcept{
            subsat_lat_=value;
            return *this;
        }
        /// @brief projection's orientation
        /// @brief ориентация проекции
        Spacial& orientation(double value) noexcept{
            orientation_=value;
            return *this;
        }
        /// @brief isometric height of satellite from Earth's center
        /// @brief изометрическая высота спутника от центра Земли
        Spacial& Z(double value) noexcept{
            Z_=value;
            return *this;
        }
        
        virtual std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept override{
            std::vector<Position> result;
            result.reserve(geodesic.size());
            if(!valid())
                return {};
            double b = datum_->b();
            double a = datum_->a();
            for(auto& [lat_deg,lon_deg]:geodesic){
                double lambda_e = lon_deg*GeographicLib::Math::degree();
                double lat_rad = lat_deg * GeographicLib::Math::degree();
                double phi_e;
                if (std::abs(lat_rad)>std::numbers::pi/2-1e-9) {
                    phi_e = (lat_deg > 0)?
                    std::numbers::pi/2:
                    (-1.)*std::numbers::pi/2;
                } else {
                    phi_e = std::atan2(b*b * std::tan(lat_rad), a*a);
                }
                double r_e = b/std::sqrt(
                    1-(a*a-b*b)/(a*a)*std::pow(std::cos(phi_e),2)
                );
                double r_1 = Z_-r_e*std::cos(phi_e)*
                    std::cos(lambda_e-subsat_lon_*GeographicLib::Math::degree());
                double r_2 = (-1.)*r_e*std::cos(phi_e)*std::sin(lambda_e-
                    subsat_lon_*GeographicLib::Math::degree());
                double r_3 = r_e*std::sin(phi_e);
                double r_n = std::sqrt(r_1*r_1+r_2*r_2+r_3*r_3);
                GeographicLib::Math::real x =
                    std::atan2((-1.)*r_2,r_1)/
                    GeographicLib::Math::degree();
                GeographicLib::Math::real y =
                    std::atan2((-1.)*r_3,r_n)/
                    GeographicLib::Math::degree();
                result.push_back(
                    Position{
                        .y_=y,
                        .x_=x
                    });
            }
            return result;
        }
        virtual std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept override{
            std::vector<Coord> result;
            result.reserve(xy.size());
            double a = datum_->a();
            double b = datum_->b();
            double a2_b2 = a*a/b/b;
            if(!valid())
                return {};
            for(auto& [x_deg,y_deg]:xy){
                double x = x_deg*GeographicLib::Math::degree();
                double y = y_deg*GeographicLib::Math::degree();
                double s_d = std::sqrt(std::pow(
                    Z_*std::cos(x)*std::cos(y),2)-
                    (std::pow(std::cos(y),2)+a2_b2*std::pow(std::sin(y),2))*
                    (Z_*Z_-a*a));
                double s_n = (Z_*std::cos(x)*std::cos(y)-s_d)/
                    (std::pow(std::cos(y),2)+a2_b2*std::pow(std::sin(y),2));
                double s_1 = Z_-s_n*std::cos(x)*std::cos(y);
                double s_2 = s_n * std::sin(x) * std::cos(y);
                double s_3 = (-1.)*s_n*std::sin(y);
                double s_xy = std::sqrt(s_1*s_1+s_2*s_2);
                Lon lon_rad = std::atan2(s_2,s_1)+
                    subsat_lon_*GeographicLib::Math::degree();
                Lat lat_rad = std::atan2(a2_b2*s_3,s_xy);
                result.push_back(Coord{
                    .lat_=lat_rad/GeographicLib::Math::degree(),
                    .lon_=lon_rad/GeographicLib::Math::degree()});
            }
            return result;
        }

        /** @brief std::abs(subsat_lat_)<=90 deg and
                std::abs(orientation_)<=360 deg and
                std::abs(subsat_lon_)<=180 deg and
                distance from satellite to Earth's center>a
        */
        virtual bool valid() const noexcept override{
            return projection().empty() &&
                std::abs(subsat_lat_)<=90. &&
                std::abs(orientation_)<=360. &&
                std::abs(subsat_lon_)<=180. &&
                Z_>datum_->a();
        }
        virtual double left_bound(Lat latitude) const noexcept override{
            return -180.;
        }
        virtual double right_bound(Lat latitude) const noexcept override{
            return 180.;
        }
        virtual double top_bound(Lon longitude) const noexcept override{
            return 90.;
        }
        virtual double bottom_bound(Lon longitude) const noexcept override{
            return -90.;
        }
        virtual bool position_in(Coord position) const noexcept override{
            return true;
        }
        std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}