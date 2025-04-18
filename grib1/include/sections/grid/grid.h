#pragma once

#include "code_tables/table_7.h"
#include "code_tables/table_8.h"
#include "albers.h"
#include "lambert.h"
#include "lat_lon.h"
#include "gnomonic.h"
#include "gaussian.h"
#include "mercator.h"
#include "millers.h"
#include "polar.h"
#include "polyconic.h"
#include "space_view.h"
#include "harmonic.h"
#include "utm.h"
#include "lambert.h"

UNION_BEG(GridDataType){
    void* empty DEF_STRUCT_VAL(NULL)
    GridDefinition<LAT_LON_GRID_EQUIDIST_CYLINDR> latlon;
    GridDefinition<ROTATED_LAT_LON> rot_latlon;
    GridDefinition<STRETCHED_LAT_LON> str_latlon;
    GridDefinition<STRETCHED_AND_ROTATED_LAT_LON> str_rot_latlon;
    GridDefinition<GAUSSIAN> gauss;
    GridDefinition<ROTATED_GAUSSIAN_LAT_LON> rot_gauss;
    GridDefinition<STRETCHED_GAUSSIAN_LAT_LON> str_gauss;
    GridDefinition<STRETCHED_ROTATED_GAUSSIAN_LAT_LON> str_rot_gauss;
    GridDefinition<LAMBERT> lambert;
    GridDefinition<OBLIQUE_LAMBERT_CONFORMAL> lambert_oblique;
    GridDefinition<UTM> utm;
    GridDefinition<ALBERS_EQUAL_AREA> albers;
    GridDefinition<SPACE_VIEW> space;
    GridDefinition<GNOMONIC> gnomonic;
    GridDefinition<SPHERICAL_HARMONIC_COEFFICIENTS> harmonic;
    GridDefinition<ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS> rot_harmonic;
    GridDefinition<STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS> str_harmonic;
    GridDefinition<STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS> str_rot_harmonic;
    GridDefinition<POLAR_STEREOGRAPH_PROJ> polar;
    GridDefinition<MILLERS_CYLINDR> millers;
    GridDefinition<SIMPLE_POLYCONIC> polyconic;
    GridDefinition<MERCATOR> mercator;

    #ifdef __cplusplus
    GridDataType():empty(NULL){} // Явное указание использовать конструктор по умолчанию
    GridDataType(unsigned char* buffer, RepresentationType T){
        switch (T)
        {
        case RepresentationType::ALBERS_EQUAL_AREA :
            albers = GridDefinition<ALBERS_EQUAL_AREA>(buffer);
            break;
        case RepresentationType::GAUSSIAN :
            gauss = GridDefinition<GAUSSIAN>(buffer);
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON :
            rot_gauss = GridDefinition<ROTATED_GAUSSIAN_LAT_LON>(buffer);
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON :
            str_gauss = GridDefinition<STRETCHED_GAUSSIAN_LAT_LON>(buffer);
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON :
            str_rot_gauss = GridDefinition<STRETCHED_ROTATED_GAUSSIAN_LAT_LON>(buffer);
            break;
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
            latlon = GridDefinition<LAT_LON_GRID_EQUIDIST_CYLINDR>(buffer);
            break;
        case RepresentationType::ROTATED_LAT_LON:
            rot_latlon = GridDefinition<ROTATED_LAT_LON>(buffer);
            break;
        case RepresentationType::STRETCHED_LAT_LON :
            str_latlon = GridDefinition<STRETCHED_LAT_LON>(buffer);
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON :
            str_rot_latlon = GridDefinition<STRETCHED_AND_ROTATED_LAT_LON>(buffer);
            break;
        case RepresentationType::LAMBERT :
            lambert = GridDefinition<LAMBERT>(buffer);
            break;
        case RepresentationType::MERCATOR :
            mercator = GridDefinition<MERCATOR>(buffer);
            break;
        case RepresentationType::MILLERS_CYLINDR :
            millers = GridDefinition<MILLERS_CYLINDR>(buffer);
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL :
            lambert_oblique = GridDefinition<OBLIQUE_LAMBERT_CONFORMAL>(buffer);
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ :
            polar = GridDefinition<POLAR_STEREOGRAPH_PROJ>(buffer);
            break;
        case RepresentationType::SIMPLE_POLYCONIC :
            polyconic = GridDefinition<SIMPLE_POLYCONIC>(buffer);
            break;
        case RepresentationType::GNOMONIC :
            gnomonic = GridDefinition<GNOMONIC>(buffer);
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
            rot_harmonic = GridDefinition<ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>(buffer);
            break;
        case RepresentationType::UTM :
            utm = GridDefinition<UTM>(buffer);
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS :
            str_harmonic = GridDefinition<STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>(buffer);
            break;
        case RepresentationType::SPACE_VIEW :
            space = GridDefinition<SPACE_VIEW>(buffer);
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS :
            harmonic = GridDefinition<SPHERICAL_HARMONIC_COEFFICIENTS>(buffer);
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
            str_rot_harmonic = GridDefinition<STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>(buffer);
            break;
        default:
            break;
        }
    }
    #endif
}
UNION_END(GridDataType)

#include <array>
#include <string_view>

constexpr std::string_view to_string_representation_type(RepresentationType rep_t){
    switch(rep_t){
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR:
            return "WGS";
            break;
        case RepresentationType::ROTATED_LAT_LON:
            return "Rotated WGS";
            break;
        case RepresentationType::STRETCHED_LAT_LON:
            return "Stretched WGS";
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON:
            return "Stretched and rotated WGS";
            break;
        case RepresentationType::GAUSSIAN:
            return "Gaussian";
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON:
            return "Rotated Gaussian";
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON:
            return "Stretched Gaussian";
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON:
            return "Stretched and rotated Gaussian";
            break;
        case RepresentationType::ALBERS_EQUAL_AREA:
            return "Albers system";
            break;
        case RepresentationType::GNOMONIC:
            return "Gnomonic system";
            break;
        case RepresentationType::LAMBERT:
            return "Lambert system";
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL:
            return "Oblique Lambert conic";
            break;
        case RepresentationType::MERCATOR:
            return "Mercator";
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Spherical harmonic coefficients";
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Rotated spherical harmonic coefficients";
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Stretched spherical harmonic coefficients";
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Stretched and rotated spherical harmonic coefficients";
            break;
        case RepresentationType::MILLERS_CYLINDR:
            return "Millers system";
            break;
        case RepresentationType::SIMPLE_POLYCONIC:
            return "Simple polyconic";
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ:
            return "Polar";
            break;
        case RepresentationType::SPACE_VIEW:
            return "Spatial";
            break;
        case RepresentationType::UTM:
            return "UTM";
            break;
        default:{
            throw std::invalid_argument("Unknown coordinate system");
        }
    }
}

constexpr std::string_view to_abbr_representation_type(RepresentationType rep_t){
    switch(rep_t){
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR:
            return "WGS";
            break;
        case RepresentationType::ROTATED_LAT_LON:
            return "rWGS";
            break;
        case RepresentationType::STRETCHED_LAT_LON:
            return "sWGS";
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON:
            return "srWGS";
            break;
        case RepresentationType::GAUSSIAN:
            return "Gauss";
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON:
            return "rGauss";
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON:
            return "sGauss";
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON:
            return "srGauss";
            break;
        case RepresentationType::ALBERS_EQUAL_AREA:
            return "Albers";
            break;
        case RepresentationType::GNOMONIC:
            return "Gnomonic";
            break;
        case RepresentationType::LAMBERT:
            return "Lambert";
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL:
            return "LambertCon";
            break;
        case RepresentationType::MERCATOR:
            return "Merc";
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
            return "SpherHarmCoef";
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "rSpherHarmCoef";
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "sSpherHarmCoef";
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "srSpherHarmCoef";
            break;
        case RepresentationType::MILLERS_CYLINDR:
            return "Millers";
            break;
        case RepresentationType::SIMPLE_POLYCONIC:
            return "SimpPolyCon";
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ:
            return "Polar";
            break;
        case RepresentationType::SPACE_VIEW:
            return "Spatial";
            break;
        case RepresentationType::UTM:
            return "UTM";
            break;
        default:{
            throw std::invalid_argument("Unknown coordinate system");
        }
    }
}

#include "types/rect.h"
#ifndef GRID_INFO
struct GridInfo{
    GridDataType data DEF_STRUCT_VAL({})
    RepresentationType rep_type DEF_STRUCT_VAL((RepresentationType)-1)
    #ifdef __cplusplus
        GridInfo(GridDataType&& gdt,RepresentationType t):data(std::move(gdt)),rep_type(t){}
        GridInfo() = default;
        bool extendable_by(const GridInfo& other) const noexcept{
            if(rep_type != other.rep_type)
                return false;
            switch (other.rep_type)
            {
            case LAT_LON_GRID_EQUIDIST_CYLINDR:
                if(data.latlon.dx!=other.data.latlon.dx ||
                    data.latlon.dy!=other.data.latlon.dy)
                    return false;
                return !(data.latlon.x1 > other.data.latlon.x2 ||
                    data.latlon.x2 < other.data.latlon.x1 || 
                    data.latlon.y1 > other.data.latlon.y2||
                    data.latlon.y2 < other.data.latlon.y1);
                break;
            default:
                std::runtime_error("Still not available");
                break;
            }
        }

        bool extend(const GridInfo& grid){
            if(!extendable_by(grid))
                return false;
            switch (grid.rep_type)
            {
            case LAT_LON_GRID_EQUIDIST_CYLINDR :
                if(grid.data.latlon.x1<data.latlon.x1)
                    data.latlon.x1 = grid.data.latlon.x1;
                if(grid.data.latlon.y2<data.latlon.y2)
                    data.latlon.y2 = grid.data.latlon.y2;
                if(grid.data.latlon.x2>data.latlon.x2)
                    data.latlon.x2 = grid.data.latlon.x2;
                if(grid.data.latlon.y1>data.latlon.y1)
                    data.latlon.y1 = grid.data.latlon.y1;
                return true;
                break;
            default:
                throw std::runtime_error("Still not available");
                break;
            }
        }
        bool operator==(const GridInfo& other) const;
        bool operator!=(const GridInfo& other) const;
    #endif
};
#endif

#include <optional>

template<>
struct std::hash<GridInfo>{
    size_t operator()(const GridInfo& data) const{
        return std::hash<uint64_t>{}((uint64_t)data.rep_type<<(sizeof(size_t)-sizeof(data.rep_type)))^
        (std::hash<std::string_view>{}(reinterpret_cast<const char*>(&data.data))>>sizeof(data.rep_type));
    }
};

template<>
struct std::hash<std::optional<GridInfo>>{
    size_t operator()(const std::optional<GridInfo>& data) const{
        if(data.has_value())
            return std::hash<GridInfo>{}(data.value());
        else return 0;
    }
};

bool pos_in_grid(const Coord& pos, const GridInfo& grid) noexcept;
int value_by_raw(const Coord& pos, const GridInfo& grid) noexcept;