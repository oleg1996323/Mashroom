#pragma once

enum RepresentationType{
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
    SPACE_VIEW=90
};

const bool is_representation[255]={
    1,1,1,1,1,
    1,1,1,1,1,
    1,0,0,1,1,
    0,0,0,0,0,
    1,0,0,0,1,
    0,0,0,0,0,
    1,0,0,0,1,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0
};

static_assert(is_representation[LAT_LON_GRID_EQUIDIST_CYLINDR]);
static_assert(is_representation[MERCATOR]);
static_assert(is_representation[GNOMONIC]);
static_assert(is_representation[LAMBERT]);
static_assert(is_representation[GAUSSIAN]);
static_assert(is_representation[POLAR_STEREOGRAPH_PROJ]);
static_assert(is_representation[UTM]);
static_assert(is_representation[SIMPLE_POLYCONIC]);
static_assert(is_representation[ALBERS_EQUAL_AREA]);
static_assert(is_representation[MILLERS_CYLINDR]);
static_assert(is_representation[ROTATED_LAT_LON]);
static_assert(is_representation[OBLIQUE_LAMBERT_CONFORMAL]);
static_assert(is_representation[ROTATED_GAUSSIAN_LAT_LON]);
static_assert(is_representation[STRETCHED_LAT_LON]);
static_assert(is_representation[STRETCHED_GAUSSIAN_LAT_LON]);
static_assert(is_representation[STRETCHED_AND_ROTATED_LAT_LON]);
static_assert(is_representation[STRETCHED_ROTATED_GAUSSIAN_LAT_LON]);
static_assert(is_representation[SPHERICAL_HARMONIC_COEFFICIENTS]);
static_assert(is_representation[ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS]);
static_assert(is_representation[STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS]);
static_assert(is_representation[STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS]);
static_assert(is_representation[SPACE_VIEW]);

