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

STRUCT_BEG(GridInfo)
{
    GridDataType data DEF_STRUCT_VAL({})
    RepresentationType rep_type DEF_STRUCT_VAL((RepresentationType)-1)
    #ifdef __cplusplus
        GridInfo(GridDataType&& gdt,RepresentationType t):data(std::move(gdt)),rep_type(t){}
    #endif
}
STRUCT_END(GridInfo)

