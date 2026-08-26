#pragma once
#include "AbstractProjection.h"

namespace projection{
    class Cylindric final:public AbstractProjectionMaker<Cylindric>{
        /// @brief top latitude
        Lat top_;
        /// @brief left longitude
        Lon left_;
        /// @brief bottom latitude
        Lat bottom_;
        /// @brief right longitude
        Lon right_;
        /// @brief latitude of natural origin
        Lat lat0_;
        /// @brief longitude of natural origin
        Lon lon0_;
        mutable double R;

        void __compute_constants() const noexcept{
            double e2 = datum_->first_eccentricity2();
            R = std::sqrt((datum_->a()*datum_->a()*(1-e2))/
                std::pow(1-e2*std::sin(lat0_*GeographicLib::Math::degree()),2));
        }
        public:
        Cylindric() = default;
        Cylindric(
            Lat top,
            Lon left,
            Lat bottom,
            Lon right,
            Lat lat_origin,
            Lon lon_origin):
            top_(top),
            left_(left),
            bottom_(bottom),
            right_(right),
            lat0_(lat_origin),
            lon0_(lon_origin){}
        Cylindric(const Cylindric& other):
            AbstractProjectionMaker(
                other),
            top_(other.top_),
            left_(other.left_),
            bottom_(other.bottom_),
            right_(other.right_),
            lat0_(other.lat0_),
            lon0_(other.lon0_){}
        Cylindric(Cylindric&& other):
        AbstractProjectionMaker(
                std::move(other)),
            top_(other.top_),
            left_(other.left_),
            bottom_(other.bottom_),
            right_(other.right_),
            lat0_(other.lat0_),
            lon0_(other.lon0_){}
        Cylindric& operator=(const Cylindric& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(other);
                top_ = other.top_;
                left_ = other.left_;
                bottom_ = other.bottom_;
                right_ = other.right_;
                lat0_ = other.lat0_;
                lon0_ = other.lon0_;
            }
            return *this;
        }
        Cylindric& operator=(Cylindric&& other) noexcept{
            if(this!=&other){
                AbstractProjectionMaker::operator=(std::move(other));
                top_ = other.top_;
                left_ = other.left_;
                bottom_ = other.bottom_;
                right_ = other.right_;
                lat0_ = other.lat0_;
                lon0_ = other.lon0_;
            }
            return *this;
        }
        bool operator==(const Cylindric& other) const noexcept{
            return AbstractProjectionMaker::operator==(other)&&
            (top_==other.top_)&&
            (left_==other.left_)&&
            (bottom_==other.bottom_)&&
            (right_==other.right_) &&
            (lat0_ == other.lat0_) &&
            (lon0_ == other.lon0_);
        }

        /// @brief top latitude
        /// @brief верхняя широта
        Lat top() const noexcept{
            return top_;
        }
        /// @brief top latitude
        /// @brief верхняя широта
        void top(Lat top) noexcept{
            top_ = top;
        }
        /// @brief bottom latitude
        /// @brief нижняя широта
        Lat bottom() const noexcept{
            return bottom_;
        }
        /// @brief bottom latitude
        /// @brief нижняя широта
        void bottom(Lat bottom) noexcept{
            bottom_ = bottom;
        }
        /// @brief left latitude
        /// @brief левая широта
        Lon left() const noexcept{
            return left_;
        }
        /// @brief left latitude
        /// @brief левая широта
        void left(Lon left) noexcept{
            left_ = left;
        }
        /// @brief right latitude
        /// @brief правая широта
        Lon right() const noexcept{
            return right_;
        }
        /// @brief right latitude
        /// @brief правая широта
        void right(Lon right) noexcept{
            right_ = right;
        }
        Lat origin_latitude() const noexcept{
            return lat0_;
        }
        Lon origin_longitude() const noexcept{
            return lon0_;
        }
        virtual std::vector<Position> forward(const std::vector<Coord>& geodesic) noexcept override{
            std::vector<Position> result;
            result.reserve(geodesic.size());
            if(!valid())
                return {};
            __compute_constants();
            for(auto& [lat_deg,lon_deg]:geodesic){
                double X = R * (lon_deg - lon0_)*GeographicLib::Math::degree()*
                    std::cos(lat0_*GeographicLib::Math::degree());
                double Y = R*lat_deg*GeographicLib::Math::degree();
                result.push_back(
                    Position{
                        .y_=Y,
                        .x_=X
                    });
            }
            return result;
        }
        virtual std::vector<Coord> inverse(const std::vector<Position>& xy) noexcept override{
            std::vector<Coord> result;
            result.reserve(xy.size());
            if(!valid())
                return {};
            __compute_constants();
            for(auto& [northing,easting]:xy){
                Lat lat_rad = northing/R;
                Lon lon_rad = lon0_*GeographicLib::Math::degree() +
                    (easting/(R*std::cos(lat0_*GeographicLib::Math::degree())));
                result.push_back(Coord{
                    .lat_=lat_rad/GeographicLib::Math::degree(),
                    .lon_=lon_rad/GeographicLib::Math::degree()});
            }
            return result;
        }

        /** @brief std::abs(top_)<=90 deg and
        /// std::abs(bottom_)<=90 deg and
        /// std::abs(left_)<=180 deg and 
        /// std::abs(right_)<=180 deg
        /// @return 
        */
        virtual bool valid() const noexcept override{
            return projection().empty() &&
                std::abs(top_)<=90. &&
                std::abs(bottom_)<=90. &&
                std::abs(left_)<=180. &&
                std::abs(right_)<=180. &&
                std::abs(lat0_)<90.;
        }
        virtual double left_bound(Lat latitude) const noexcept override{
            return left_;
        }
        virtual double right_bound(Lat latitude) const noexcept override{
            return right_;
        }
        virtual double top_bound(Lon longitude) const noexcept override{
            return top_;
        }
        virtual double bottom_bound(Lon longitude) const noexcept override{
            return bottom_;
        }
        virtual bool position_in(Coord position) const noexcept override{
            return std::abs(position.lon_-90.) >= std::abs(left_-180.) && 
                std::abs(position.lon_-90.) <= std::abs(right_ - 180.) &&
                std::abs(position.lat_-90.) >= std::abs(bottom_ -90.) &&
                std::abs(position.lat_-90.) <= std::abs(top_-90.);

                /*
                    // Нормализуем долготу точки в диапазон [-180, 180]
                    double lon = position.lon_;
                    while (lon > 180.0) lon -= 360.0;
                    while (lon < -180.0) lon += 360.0;

                    // Нормализуем границы
                    double left = left_;
                    double right = right_;
                    while (left > 180.0) left -= 360.0;
                    while (left < -180.0) left += 360.0;
                    while (right > 180.0) right -= 360.0;
                    while (right < -180.0) right += 360.0;

                    // Проверка: если left <= right (обычный случай)
                    if (left <= right) {
                        return lon >= left && lon <= right &&
                            position.lat_ >= bottom_ && position.lat_ <= top_;
                    }
                    // Если left > right (пересечение 180-го меридиана)
                    else {
                        return (lon >= left || lon <= right) &&
                            position.lat_ >= bottom_ && position.lat_ <= top_;
                    }
                */
        }
        // Клонирование для полиморфного копирования
        virtual std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}