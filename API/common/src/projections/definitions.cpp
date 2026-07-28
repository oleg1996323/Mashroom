#include "common/projections/definitions.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstddef>
#include <cstdint>
#include <memory>

enum RepresentationType:uint8_t{
    LAT_LON_GRID_EQUIDIST_CYLINDR = 0,
    MERCATOR = 1,
    GNOMONIC = 2,
    LAMBERT = 3,
    GAUSSIAN = 4,
    POLAR_STEREOGRAPH_PROJ = 5,
    UTM=6,
    SIMPLE_POLYCONIC=7,
    ALBERS_EQUAL_AREA=8,
    MILLERS_CYLINDR=9,
    ROTATED_LAT_LON=10,
    OBLIQUE_LAMBERT_CONFORMAL = 13,
    ROTATED_GAUSSIAN_LAT_LON=ROTATED_LAT_LON+GAUSSIAN,
    STRETCHED_LAT_LON=20,
    STRETCHED_GAUSSIAN_LAT_LON=STRETCHED_LAT_LON+GAUSSIAN,
    STRETCHED_AND_ROTATED_LAT_LON=STRETCHED_LAT_LON+ROTATED_LAT_LON,
    STRETCHED_ROTATED_GAUSSIAN_LAT_LON=STRETCHED_AND_ROTATED_LAT_LON+GAUSSIAN,
    SPHERICAL_HARMONIC_COEFFICIENTS = 50,
    ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS=SPHERICAL_HARMONIC_COEFFICIENTS+ROTATED_LAT_LON,
    STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS=STRETCHED_LAT_LON+SPHERICAL_HARMONIC_COEFFICIENTS,
    STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS=SPHERICAL_HARMONIC_COEFFICIENTS+\
                                                        ROTATED_LAT_LON+\
                                                        STRETCHED_LAT_LON,
    SPACE_VIEW=90, //22
    UNDEF_GRID = UINT8_MAX
};

std::unordered_map< std::string,
                    std::unordered_set<
                    std::unique_ptr<
                        projection::CommonOptions>>> 
    projections_options_;


std::error_code add_projection(
    std::string name,
    std::unique_ptr<projection::CommonOptions> options)
{
    if(name.empty())
        return std::make_error_code(std::errc::invalid_argument);
    if(projections_options_[std::move(name)].insert(std::move(options)).second)
        return {};
    else return std::make_error_code(std::errc::invalid_argument);//already exists
}

std::unique_ptr<projection::CommonOptions> projection_options(
        const std::string name,
        const std::string& format,
        bool rotatable,
        bool stretchable)
{

}



void define_from_json(const std::filesystem::path& file){

}