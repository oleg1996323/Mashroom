#pragma once
#include "common/AbstractProjection.h"
#include "OsterLib/types/coord.h"
#include <utility>

namespace projection{
    class Albers final:public AbstractProjection{
        /// @brief false easting
        /// @brief смещение по долготе
        Lon easting_ = 0;
        /// @brief false northing
        /// @brief Смещение по широте
        Lat northing_ = 0;
        /// @brief central meridian
        /// @brief Центральный меридиан
        Lon central_meridian_ = 0;

        /// @brief standards latitudes
        /// @brief Стандартные параллели
        std::pair<Lat,Lat> std_latitudes_;
        /// @brief latitude origin
        /// @brief Широта начальной точки
        Lat latitude_orig_ = 0;
        public:
        Albers(std::string_view name):
            AbstractProjection(name){}
        Albers(Lon false_easting,
                Lat false_northing,
                Lon central_meridian,
                Lat standard_latitude_1,
                Lat standard_latitude_2,
                Lat latitude_origin) noexcept:
                AbstractProjection("albers"),
                easting_(false_easting),
                northing_(false_northing),
                central_meridian_(central_meridian),
                std_latitudes_(standard_latitude_1,standard_latitude_2),
                latitude_orig_(latitude_origin){}
        Albers(const Albers& other):
            AbstractProjection(
                other.name_),
            easting_(other.easting_),
            northing_(other.northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        Albers(Albers&& other):
        AbstractProjection(
                std::move(other.name_)),
            easting_(other.easting_),
            northing_(other.northing_),
            central_meridian_(other.central_meridian_),
            std_latitudes_(other.std_latitudes_),
            latitude_orig_(other.latitude_orig_){}
        Albers& operator=(const Albers& other){
            if(this!=&other){
                AbstractProjection::operator=(*this);
                easting_ = other.easting_;
                northing_ = other.northing_;
                central_meridian_ = other.central_meridian_;
                std_latitudes_ = other.std_latitudes_;
                latitude_orig_ = other.latitude_orig_;
            }
            return *this;
        }
        Albers& operator=(Albers&& other) noexcept{
            if(this!=&other){
                AbstractProjection::operator=(std::move(*this));
                easting_ = other.easting_;
                northing_ = other.northing_;
                central_meridian_ = other.central_meridian_;
                std_latitudes_ = other.std_latitudes_;
                latitude_orig_ = other.latitude_orig_;
            }
            return *this;
        }
        bool operator==(const Albers& other) const noexcept{
            bool result = (easting_==other.easting_)&&
            (northing_==other.northing_)&&
            (central_meridian_==other.central_meridian_)&&
            (std_latitudes_==other.std_latitudes_)&&
            (latitude_orig_==other.latitude_orig_)&&
            (rotation_==other.rotation_)&&
            (stretch_==other.stretch_);
            return result;
        }
        /// @brief false easting
        /// @brief смещение по долготе
        Lon false_easting() const noexcept{
            return easting_;
        }
        /// @brief false northing
        /// @brief Смещение по широте
        Lat false_northing() const noexcept{
            return northing_;
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
            easting_=value;
            return *this;
        }
        /// @brief false northing
        /// @brief Смещение по широте
        Albers& false_northing(Lat value) noexcept{
            northing_=value;
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

        std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}