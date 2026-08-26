#pragma once
#include "AbstractProjection.h"
#include "OsterLib/types/coord.h"
#include <utility>
#include <GeographicLib/Math.hpp>

namespace projection{
    class Albers final:public AbstractProjectionMaker<Albers>{
        /// @brief false easting
        /// @brief смещение по долготе
        Lon false_easting_ = 0;
        /// @brief false northing
        /// @brief Смещение по широте
        Lat false_northing_ = 0;
        /// @brief central meridian
        /// @brief Центральный меридиан
        Lon central_meridian_ = 0;

        /// @brief standards latitudes
        /// @brief Стандартные параллели
        std::pair<Lat,Lat> std_latitudes_;
        /// @brief latitude origin
        /// @brief Широта начальной точки
        Lat latitude_orig_ = 0;
        mutable double rho0;
        mutable double C;
        mutable double n;
        mutable double m1;
        mutable double m2;
        mutable double alpha;
        mutable double alpha0;
        mutable double alpha1;
        mutable double alpha2;
        mutable double beta_K;
        void __compute_constants__() const noexcept{
            double e2 = datum_->first_eccentricity2();
            double e = datum_->first_eccentricity();
            m1 = std::cos(std_latitudes_.first*GeographicLib::Math::degree())/
                std::sqrt(1-e2*std::pow(
                    std::sin(std_latitudes_.first*GeographicLib::Math::degree()),2));
            m2 = std::cos(std_latitudes_.second*GeographicLib::Math::degree())/
                std::sqrt(1-e2*std::pow(
                    std::sin(std_latitudes_.second*GeographicLib::Math::degree()),2));
            if(e>1e-9){
                alpha0 = (1 - e2)*((std::sin(latitude_orig_*GeographicLib::Math::degree())/
                    (1-e2*std::pow(std::sin(latitude_orig_*GeographicLib::Math::degree()),2)))-
                    1/(2*e)*
                    std::log((1-e*std::sin(latitude_orig_*GeographicLib::Math::degree()))/
                    (1+e*std::sin(latitude_orig_*GeographicLib::Math::degree()))));
                alpha1 = (1 - e2)*((std::sin(std_latitudes_.first*GeographicLib::Math::degree())/
                    (1-e2*std::pow(std::sin(std_latitudes_.first*GeographicLib::Math::degree()),2)))-
                    1/(2*e)*
                    std::log((1-e*std::sin(std_latitudes_.first*GeographicLib::Math::degree()))/
                    (1+e*std::sin(std_latitudes_.first*GeographicLib::Math::degree()))));
                alpha2 = (1 - e2)*((std::sin(std_latitudes_.second*GeographicLib::Math::degree())/
                    (1-e2*std::pow(std::sin(std_latitudes_.second*GeographicLib::Math::degree()),2)))-
                    1/(2*e)*
                    std::log((1-e*std::sin(std_latitudes_.second*GeographicLib::Math::degree()))/
                    (1+e*std::sin(std_latitudes_.second*GeographicLib::Math::degree()))));
            }
            else{
                alpha0 = std::sin(latitude_orig_*GeographicLib::Math::degree());
                alpha1 = std::sin(std_latitudes_.first*GeographicLib::Math::degree());
                alpha2 = std::sin(std_latitudes_.second*GeographicLib::Math::degree());
            }
            if(std::abs(alpha1-alpha2)>1e-9)
                n = (std::pow(m1,2)-std::pow(m2,2))/(alpha2-alpha1);
            else n = std::sin(std_latitudes_.first*GeographicLib::Math::degree());
            C = std::pow(m1,2)+(n*alpha1);
            if(std::abs(n)>1e-9){
                n = 0.;
                rho0 = (datum_->a()*std::sqrt(C-n*alpha0))/n;
            }
            else return;
            if(e>1e-9)
                beta_K = 1-(1-e2)/(2*e)*std::log((1-e)/(1+e));
            else beta_K = 1;
        }
        public:
        Albers()=default;
        Albers(const Albers& other):
            AbstractProjectionMaker(
                other),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        Albers(Albers&& other):
        AbstractProjectionMaker(
                std::move(other)),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        Albers& operator=(const Albers& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(other);
                false_easting_ = other.false_easting_;
                false_northing_ = other.false_northing_;
                central_meridian_ = other.central_meridian_;
                std_latitudes_ = other.std_latitudes_;
                latitude_orig_ = other.latitude_orig_;
            }
            return *this;
        }
        Albers& operator=(Albers&& other) noexcept{
            if(this!=&other){
                AbstractProjection::operator=(std::move(other));
                false_easting_ = other.false_easting_;
                false_northing_ = other.false_northing_;
                central_meridian_ = other.central_meridian_;
                std_latitudes_ = other.std_latitudes_;
                latitude_orig_ = other.latitude_orig_;
            }
            return *this;
        }
        bool operator==(const Albers& other) const noexcept{
            return AbstractProjection::operator==(other)&&
            (false_easting_==other.false_easting_)&&
            (false_northing_==other.false_northing_)&&
            (central_meridian_==other.central_meridian_)&&
            (std_latitudes_==other.std_latitudes_)&&
            (latitude_orig_==other.latitude_orig_);
        }
        /// @brief false easting
        /// @brief смещение по долготе
        Lon false_easting() const noexcept{
            return false_easting_;
        }
        /// @brief false northing
        /// @brief Смещение по широте
        Lat false_northing() const noexcept{
            return false_northing_;
        }
        /// @brief central meridian
        /// @brief Центральный меридиан
        Lon central_meridian() const noexcept{
            return central_meridian_;
        }
        /// @brief first standard latitude
        /// @brief Первая стандартная параллель
        Lat first_central_latitude() const noexcept{
            return std_latitudes_.first;
        }
        /// @brief second standard latitude
        /// @brief Вторая стандартная параллель
        Lat second_central_latitude() const noexcept{
            return std_latitudes_.second;
        }
        /// @brief latitude origin
        /// @brief Широта начальной точки
        Lat latitude_origin() const noexcept{
            return latitude_orig_;
        }
        /// @brief false easting
        /// @brief смещение по долготе
        Albers& false_easting(Lon value) noexcept{
            false_easting_=value;
            return *this;
        }
        /// @brief false northing
        /// @brief Смещение по широте
        Albers& false_northing(Lat value) noexcept{
            false_northing_=value;
            return *this;
        }
        /// @brief central meridian
        /// @brief Центральный меридиан
        Albers& central_meridian(Lon value) noexcept{
            central_meridian_=value;
            return *this;
        }
        /// @brief first standard latitude
        /// @brief Первая стандартная параллель
        Albers& first_central_latitude(Lat value) noexcept{
            std_latitudes_.first=value;
            return *this;
        }
        /// @brief second standard latitude
        /// @brief Вторая стандартная параллель
        Albers& second_central_latitude(Lat value) noexcept{
            std_latitudes_.second=value;
            return *this;
        }
        /// @brief latitude origin
        /// @brief Широта начальной точки
        Albers& latitude_origin(Lat value) noexcept{
            latitude_orig_=value;
            return *this;
        }

        virtual std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept override{
            std::vector<Position> result;
            result.reserve(geodesic.size());
            __compute_constants__();
            if(!valid())
                return {};
            double e2 = datum_->first_eccentricity2();
            double e = datum_->first_eccentricity();
            for(auto& [lat,lon]:geodesic){
                double alpha;
                if(e>1e-9)
                    alpha = (1 - e2)*((std::sin(lat*GeographicLib::Math::degree())/
                    (1-e2*std::pow(std::sin(lat*GeographicLib::Math::degree()),2)))-
                    1/(2*e)*
                    std::log((1-e*std::sin(lat*GeographicLib::Math::degree()))/
                    (1+e*std::sin(lat*GeographicLib::Math::degree()))));
                else alpha = std::sin(lat*GeographicLib::Math::degree());
                double rho = (datum_->a()*std::sqrt(C-n*alpha))/n;
                double thetha = n*(lon*GeographicLib::Math::degree()-
                    central_meridian_*GeographicLib::Math::degree());
                result.push_back(
                    Position{
                        .y_=false_northing_ + rho0 - (rho*std::cos(thetha)),
                        .x_=false_easting_ + (rho*std::sin(thetha))
                    });
            }
            return result;
        }
        virtual std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept override{
            std::vector<Coord> result;
            result.reserve(xy.size());
            if(!valid())
                return {};
            double e2 = datum_->first_eccentricity2();
            for(auto& [northing,easting]:xy){
                double dx = easting - false_easting_;
                double dy = rho0-(northing-false_northing_);
                double rho = std::sqrt(dx*dx+dy*dy);
                double alpha = (C-std::pow(rho,2)*std::pow(northing,2)/
                    std::pow(datum_->a(),2))/n;
                double beta = std::asin(std::clamp(alpha, -1.0, 1.0)/beta_K);
                double theta;
                if(n>0)
                    theta = std::atan2(dx,dy);
                else theta = std::atan2(-dx,-dy);
                GeographicLib::Math::real lat_rad = 
                    beta+((e2/3+31*e2*e2/180+517*e2*e2*e2/5040)*std::sin(2*beta))+
                    ((23*e2*e2/360+251*e2*e2*e2/3780)*std::sin(4*beta))+
                    (761*e2*e2*e2/45360)*std::sin(6*beta);
                GeographicLib::Math::real lon_rad = 
                    central_meridian_*GeographicLib::Math::degree()+
                    theta/n;
                result.push_back(Coord{
                    .lat_=lat_rad/GeographicLib::Math::degree(),
                    .lon_=lon_rad/GeographicLib::Math::degree()});
            }
            return result;
        }

        virtual bool valid() const noexcept override{
            return projection().empty() &&
                (std::abs(n)>1e-9) && 
                (std::abs(beta_K)>1e-9);
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