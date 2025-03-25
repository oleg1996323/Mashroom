#pragma once

//https://codes.ecmwf.int/grib/format/grib1/level/3/

enum LevelsTags{
    GROUND_OR_WATER_SURFACE = 1,
    CLOUD_BASE_LEVEL = 2,
    LEVEL_OF_CLOUD_TOPS = 3,
    LEVEL_OF_0C_ISOTHERM = 4,
    LEVEL_OF_ADIABATIC_CONDENSATION_LIFTED_FROM_SURFACE = 5,
    MAXIMUM_WIND_LEVEL = 6,
    TROPOPAUSE = 7,
    NOMINAL_TOP_OF_ATMOSPHERE = 8,
    SEA_BOTTOM = 9,
    ISOBARIC_SURFACE = 100,
    LAYER_BETWEEN_TWO_ISOBARIC_SURFACES = 101,
    MEAN_SEA_LEVEL = 102,
    SPECIFIED_ALTITUDE_ABOVE_MEAN_SEA_LEVEL = 103,
    LAYER_BETWEEN_TWO_SPECIFIED_ALTITUDES = 104,
    SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND = 105,
    LAYER_BETWEEN_TWO_SPECIFIED_HEIGHT_LEVELS = 106,
    SIGMA_LEVEL = 107,
    LAYER_BETWEEN_TWO_SIGMA_LEVELS = 108,
    HYBRID_LEVEL = 109,
    LAYER_BETWEEN_TWO_HYBRID_LEVELS = 110,
    DEPTH_BELOW_LAND_SURFACE = 111,
    LAYER_BETWEEN_TWO_DEPTHS_BELOW_LAND = 112,
    ISENTROPIC_THETA_LEVEL = 113,
    LAYER_BETWEEN_TWO_ISENTROPIC_LEVELS = 114,
    LEVEL_AT_SPECIFIED_PRESSURE_DIFFERENCE = 115,
    LAYER_BETWEEN_TWO_LEVELS_AT_SPECIFIED_PRESSURE_DIF_FROM_GROUND_TO_LEVEL = 116,
    POTENTIAL_VORTICITY_SURFACE = 117,
    ETA_LEVEL = 119,
    LAYER_BETWEEN_TWO_ETA_LEVELS = 120,
    LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_HP = 121,
    SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND_HP = 125,
    SATELLITE_SPECTRAL_BAND = 127,
    LAYER_BETWEEN_TWO_SIGMA_LEVELS_HP = 128,
    LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_MP = 141,
    DEPTH_BELOW_SEA_LEVEL = 160,
    ISOTHERMAL_LEVEL = 20,
    ENTIRE_ATMOSPHERE = 200,
    ENTIRE_OCEAN = 201,
    ISOBARIC_SURFACE_HP = 210
};

#include <stdbool.h>
constexpr bool levels_10_octets[255]={
    0,1,1,1,1,
    1,1,1,1,1,
    0,0,0,0,0,
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
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,0,1,
    1,1,0,0,0,
    1,0,1,1,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,1,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,0,0,0,
    0,0,0,0,0,
    1,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0
};

static_assert(levels_10_octets[GROUND_OR_WATER_SURFACE]);
static_assert(levels_10_octets[CLOUD_BASE_LEVEL]);
static_assert(levels_10_octets[LEVEL_OF_CLOUD_TOPS]);
static_assert(levels_10_octets[LEVEL_OF_0C_ISOTHERM]);
static_assert(levels_10_octets[LEVEL_OF_ADIABATIC_CONDENSATION_LIFTED_FROM_SURFACE]);
static_assert(levels_10_octets[MAXIMUM_WIND_LEVEL]);
static_assert(levels_10_octets[TROPOPAUSE]);
static_assert(levels_10_octets[NOMINAL_TOP_OF_ATMOSPHERE]);
static_assert(levels_10_octets[SEA_BOTTOM]);
static_assert(levels_10_octets[ISOBARIC_SURFACE]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES]);
static_assert(levels_10_octets[MEAN_SEA_LEVEL]);
static_assert(levels_10_octets[SPECIFIED_ALTITUDE_ABOVE_MEAN_SEA_LEVEL]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_SPECIFIED_ALTITUDES]);
static_assert(levels_10_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_SPECIFIED_HEIGHT_LEVELS]);
static_assert(levels_10_octets[SIGMA_LEVEL]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS]);
static_assert(levels_10_octets[DEPTH_BELOW_LAND_SURFACE]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_DEPTHS_BELOW_LAND]);
static_assert(levels_10_octets[ISENTROPIC_THETA_LEVEL]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_ISENTROPIC_LEVELS]);
static_assert(levels_10_octets[LEVEL_AT_SPECIFIED_PRESSURE_DIFFERENCE]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_LEVELS_AT_SPECIFIED_PRESSURE_DIF_FROM_GROUND_TO_LEVEL]);
static_assert(levels_10_octets[POTENTIAL_VORTICITY_SURFACE]);
static_assert(levels_10_octets[ETA_LEVEL]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_ETA_LEVELS]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_HP]);
static_assert(levels_10_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND_HP]);
static_assert(levels_10_octets[SATELLITE_SPECTRAL_BAND]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS_HP]);
static_assert(levels_10_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_MP]);
static_assert(levels_10_octets[DEPTH_BELOW_SEA_LEVEL]);
static_assert(levels_10_octets[ISOTHERMAL_LEVEL]);
static_assert(levels_10_octets[ENTIRE_ATMOSPHERE]);
static_assert(levels_10_octets[ENTIRE_OCEAN]);
static_assert(levels_10_octets[ISOBARIC_SURFACE_HP]);

#include <stdint.h>
constexpr uint8_t levels_11_octets[255]={
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
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
    2,1,0,2,1,
    2,1,2,1,2,
    1,2,1,2,1,
    2,1,1,0,2,
    1,1,0,0,0,
    2,0,2,1,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,1,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    2,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    2,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0
};
static_assert(levels_11_octets[GROUND_OR_WATER_SURFACE]==0);
static_assert(levels_11_octets[CLOUD_BASE_LEVEL]==0);
static_assert(levels_11_octets[LEVEL_OF_CLOUD_TOPS]==0);
static_assert(levels_11_octets[LEVEL_OF_0C_ISOTHERM]==0);
static_assert(levels_11_octets[LEVEL_OF_ADIABATIC_CONDENSATION_LIFTED_FROM_SURFACE]==0);
static_assert(levels_11_octets[MAXIMUM_WIND_LEVEL]==0);
static_assert(levels_11_octets[TROPOPAUSE]==0);
static_assert(levels_11_octets[NOMINAL_TOP_OF_ATMOSPHERE]==0);
static_assert(levels_11_octets[SEA_BOTTOM]==0);
static_assert(levels_11_octets[ISOBARIC_SURFACE]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES]==1);
static_assert(levels_11_octets[MEAN_SEA_LEVEL]==0);
static_assert(levels_11_octets[SPECIFIED_ALTITUDE_ABOVE_MEAN_SEA_LEVEL]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_SPECIFIED_ALTITUDES]==1);
static_assert(levels_11_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_SPECIFIED_HEIGHT_LEVELS]==1);
static_assert(levels_11_octets[SIGMA_LEVEL]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS]==1);
static_assert(levels_11_octets[DEPTH_BELOW_LAND_SURFACE]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_DEPTHS_BELOW_LAND]==1);
static_assert(levels_11_octets[ISENTROPIC_THETA_LEVEL]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ISENTROPIC_LEVELS]==1);
static_assert(levels_11_octets[LEVEL_AT_SPECIFIED_PRESSURE_DIFFERENCE]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_LEVELS_AT_SPECIFIED_PRESSURE_DIF_FROM_GROUND_TO_LEVEL]==1);
static_assert(levels_11_octets[POTENTIAL_VORTICITY_SURFACE]==1);
static_assert(levels_11_octets[ETA_LEVEL]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ETA_LEVELS]==1);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_HP]==1);
static_assert(levels_11_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND_HP]==2);
static_assert(levels_11_octets[SATELLITE_SPECTRAL_BAND]==2);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS_HP]==1);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_MP]==1);
static_assert(levels_11_octets[DEPTH_BELOW_SEA_LEVEL]==2);
static_assert(levels_11_octets[ISOTHERMAL_LEVEL]==1);
static_assert(levels_11_octets[ENTIRE_ATMOSPHERE]==0);
static_assert(levels_11_octets[ENTIRE_OCEAN]==0);
static_assert(levels_11_octets[ISOBARIC_SURFACE_HP]==2);

constexpr uint8_t levels_12_octets[255]={
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
    0,1,0,0,1,
    0,1,0,1,0,
    1,0,1,0,1,
    0,1,0,0,0,
    1,1,0,0,0,//120
    0,0,0,1,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,1,0,0,0,
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

static_assert(levels_12_octets[GROUND_OR_WATER_SURFACE]==0);
static_assert(levels_12_octets[CLOUD_BASE_LEVEL]==0);
static_assert(levels_12_octets[LEVEL_OF_CLOUD_TOPS]==0);
static_assert(levels_12_octets[LEVEL_OF_0C_ISOTHERM]==0);
static_assert(levels_12_octets[LEVEL_OF_ADIABATIC_CONDENSATION_LIFTED_FROM_SURFACE]==0);
static_assert(levels_12_octets[MAXIMUM_WIND_LEVEL]==0);
static_assert(levels_12_octets[TROPOPAUSE]==0);
static_assert(levels_12_octets[NOMINAL_TOP_OF_ATMOSPHERE]==0);
static_assert(levels_12_octets[SEA_BOTTOM]==0);
static_assert(levels_12_octets[ISOBARIC_SURFACE]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES]==1);
static_assert(levels_12_octets[MEAN_SEA_LEVEL]==0);
static_assert(levels_12_octets[SPECIFIED_ALTITUDE_ABOVE_MEAN_SEA_LEVEL]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_SPECIFIED_ALTITUDES]==1);
static_assert(levels_12_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_SPECIFIED_HEIGHT_LEVELS]==1);
static_assert(levels_12_octets[SIGMA_LEVEL]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS]==1);
static_assert(levels_12_octets[DEPTH_BELOW_LAND_SURFACE]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_DEPTHS_BELOW_LAND]==1);
static_assert(levels_12_octets[ISENTROPIC_THETA_LEVEL]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_ISENTROPIC_LEVELS]==1);
static_assert(levels_12_octets[LEVEL_AT_SPECIFIED_PRESSURE_DIFFERENCE]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_LEVELS_AT_SPECIFIED_PRESSURE_DIF_FROM_GROUND_TO_LEVEL]==1);
static_assert(levels_12_octets[POTENTIAL_VORTICITY_SURFACE]==0);
static_assert(levels_12_octets[ETA_LEVEL]==0);
static_assert(levels_11_octets[LAYER_BETWEEN_TWO_ETA_LEVELS]==1);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_HP]==1);
static_assert(levels_12_octets[SPECIFIED_HEIGHT_LEVEL_ABOVE_GROUND_HP]==0);
static_assert(levels_12_octets[SATELLITE_SPECTRAL_BAND]==0);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_SIGMA_LEVELS_HP]==1);
static_assert(levels_12_octets[LAYER_BETWEEN_TWO_ISOBARIC_SURFACES_MP]==1);
static_assert(levels_12_octets[DEPTH_BELOW_SEA_LEVEL]==0);
static_assert(levels_12_octets[ISOTHERMAL_LEVEL]==0);
static_assert(levels_12_octets[ENTIRE_ATMOSPHERE]==0);
static_assert(levels_12_octets[ENTIRE_OCEAN]==0);
static_assert(levels_12_octets[ISOBARIC_SURFACE_HP]==0);