#pragma once
#include "code_tables/table_0.h"



// typedef enum DATA_TYPE{
//     UNDEF_DATA_TYPE,
//     ERA5,
//     ERA5_INTERIM,
//     CAMS,
//     SEAS5,
//     ENS,
//     GMAO,
//     TRMM,
//     GPM,
//     MODIS,
//     NCEP,
//     GHCN,
//     GFS,
//     CMIP,
//     C3S,
//     CAMS,
//     CMEMS,
//     JRA55,
//     GSM,
//     HadGEM,
//     ERA20C,
//     UM,
//     ICON,
//     COSMO,
//     GIDROMET_FORECAST,
//     GIDROMET_REANALYSIS,
//     CRA,
//     GRAPES,
//     GPCP,
//     CRU,
//     ISCCP,
//     GLDAS
// }Collection;



/*
#define NMC			7
#define JMA			34
#define ECMWF			98
#define DWD			78
#define CMC			54
#define CPTEC			46
#define CHM			146
#define LAMI			200
*/



typedef struct DATA_ITEM{
    Discipline discipline;
    CategoryParameter category;
    Parameter parameter;
}Item;

// const Item Items[200]={
//     {METEO,category_TEMPERATURE,1}
// };