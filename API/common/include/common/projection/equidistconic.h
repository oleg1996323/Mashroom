#pragma once
#include "AbstractProjection.h"
#include "OsterLib/types/coord.h"
#include <utility>
#include <algorithm>
#include <numeric>
#include <numbers>

namespace projection{
    class EquidistConic final:public AbstractProjectionMaker<EquidistConic>{
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

        mutable double m1;
        mutable double m2;
        mutable double M1;
        mutable double M2;
        mutable double MF;
        mutable double n;
        mutable double G;
        mutable double rF;

        double ellipse_length (Lat lat) const noexcept{
            double e2 = datum_->first_eccentricity2();
            return datum_->a()*((1-e2/4-
                3*std::pow(e2,2)/64-
                5*std::pow(e2,3)/256)*lat-
                (3*e2/8+
                3*std::pow(e2,2)/32+
                45*std::pow(e2,3)/1024)*std::sin(2*lat)+
                (15*std::pow(e2,2)/256+
                45*std::pow(e2,3)/1024)*
                std::sin(4*lat)-
                (35*std::pow(e2,3)/3072)*
                std::sin(6*lat));
        }
        void __compute_constants__() const noexcept{
            double e2 = datum_->first_eccentricity2();
            if(e2>1e-9){
                m1 = std::cos(std_latitudes_.first*GeographicLib::Math::degree())/
                    std::pow(1-e2*
                    std::pow(std::sin(std_latitudes_.first*GeographicLib::Math::degree()),2),0.5);
                m2 = std::cos(std_latitudes_.second*GeographicLib::Math::degree())/
                    std::pow(1-e2*
                    std::pow(std::sin(std_latitudes_.second*GeographicLib::Math::degree()),2),0.5);
            }
            else{
                m1 = std::cos(std_latitudes_.first*GeographicLib::Math::degree());
                m2 = std::cos(std_latitudes_.second*GeographicLib::Math::degree());
            }
            
            M1 = ellipse_length(std_latitudes_.first*GeographicLib::Math::degree());
            M2 = ellipse_length(std_latitudes_.second*GeographicLib::Math::degree());
                //длина дуги до большей станд. широты
            MF = ellipse_length(latitude_orig_*GeographicLib::Math::degree());
            if(std::abs(std_latitudes_.first - std_latitudes_.second) > 
                1e-9){
                n = datum_->a()*(m1-m2)/(M2-M1); //конусность
            }
            else n = std::sin(std_latitudes_.first*GeographicLib::Math::degree());
            if(n>1e-9){
                n = 0;
                G = (m1/n)+(M1/datum_->a());
            }
            else return;
            rF = datum_->a()*G-MF;
        }
        public:
        EquidistConic()=default;
        EquidistConic(const EquidistConic& other):
            AbstractProjectionMaker(
                other),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        EquidistConic(EquidistConic&& other):
            AbstractProjectionMaker(
                std::move(other)),
            false_easting_(other.false_easting_),
            false_northing_(other.false_northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        EquidistConic& operator=(const EquidistConic& other){
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
        EquidistConic& operator=(EquidistConic&& other) noexcept{
            if(this!=&other){
                AbstractProjectionMaker::operator=(std::move(other));
                false_easting_ = other.false_easting_;
                false_northing_ = other.false_northing_;
                central_meridian_ = other.central_meridian_;
                std_latitudes_ = other.std_latitudes_;
                latitude_orig_ = other.latitude_orig_;
            }
            return *this;
        }
        bool operator==(const EquidistConic& other) const noexcept{
            return AbstractProjectionMaker::operator==(other)&&(false_easting_==other.false_easting_)&&
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
        EquidistConic& false_easting(Lon value) noexcept{
            false_easting_=value;
            return *this;
        }
        /// @brief false northing
        /// @brief Смещение по широте
        EquidistConic& false_northing(Lat value) noexcept{
            false_northing_=value;
            return *this;
        }
        /// @brief central meridian
        /// @brief Центральный меридиан
        EquidistConic& central_meridian(Lon value) noexcept{
            central_meridian_=value;
            return *this;
        }
        /// @brief first standard latitude
        /// @brief Первая стандартная параллель
        EquidistConic& first_central_latitude(Lat value) noexcept{
            std_latitudes_.first=value;
            return *this;
        }
        /// @brief second standard latitude
        /// @brief Вторая стандартная параллель
        EquidistConic& second_central_latitude(Lat value) noexcept{
            std_latitudes_.second=value;
            return *this;
        }
        /// @brief latitude origin
        /// @brief Широта начальной точки
        EquidistConic& latitude_origin(Lat value) noexcept{
            latitude_orig_=value;
            return *this;
        }
        virtual std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept override{
            std::vector<Position> result;
            result.reserve(geodesic.size());
            __compute_constants__();
            if(!valid())
                return {};
            for(auto& [lat,lon]:geodesic){
                double M = ellipse_length(lat*GeographicLib::Math::degree());
                double r = datum_->a()*G-M;
                double thetha = n*(lon-central_meridian_*GeographicLib::Math::degree());
                result.push_back(
                    Position{
                        .y_=false_northing_+rF-r*std::cos(thetha),
                        .x_=false_easting_+r*std::sin(thetha)
                    });
            }
            return result;
        }
        virtual std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept override{
            std::vector<Coord> result;
            result.reserve(xy.size());
            double e2 = datum_->first_eccentricity2();
            double e1 = datum_->second_eccentricity();
            __compute_constants__();
            if(!valid())
                return {};
            for(auto& [northing,easting]:xy){
                double dx = easting-false_easting_;
                double dy = rF - (northing-false_northing_);
                double r_ = (n<0?-1:1)*std::sqrt(dx*dx+dy*dy);
                double theta;
                if(n<0)
                    theta = std::atan2(-dx,-dy);
                else theta = std::atan2(dx,dy);
                double M = datum_->a()*G-r_;
                double mu = M/
                    (datum_->a()*(1-e2/4-
                    3*std::pow(e2,2)/64-
                    5*std::pow(e2,3)/256));
                
                GeographicLib::Math::real lat_rad = mu+
                (3*e1/2-27*std::pow(e1,3)/32)*std::sin(2*mu)+
                    (21*std::pow(e1,2)/16-55*std::pow(e1,4)/32)*std::sin(4*mu)+
                    (151*std::pow(e1,3)/96)*std::sin(6*mu)+
                    (1097*std::pow(e1,4)/512)*std::sin(8*mu);
                GeographicLib::Math::real lon_rad=
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
                    std::abs(n)>1e-9;
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
        virtual bool position_in(Coord) const noexcept override{
            return true;
        }
        std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}