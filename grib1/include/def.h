#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_2.h"
#include "types/date.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

typedef enum {
    NONE_ERR,
    DATA_EMPTY,
    OPEN_ERROR,
    READ_POS,
    MEMORY_ERROR,
    BAD_FILE,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION
}ErrorCodeData;

static const char* err_code_data[11]={
    "Correct",
    "Error openning",
    "Read position error",
    "Memory allocation error (not enough memory)",
    "Corrupted file",
    "Missing grib record",
    "Ran out of memory",
    "Could not read to end of record",
    "Ran out of data or bad file",
    "Len of grib message is inconsistent",
    "End section missed"
};

typedef enum DATA_FORMAT{
    NONE,
    BINARY,
    TEXT, 
    IEEE, 
    GRIB
}DataFormat;

typedef struct VALUE_BY_COORD{
    float value;
    double lat;
    double lon;
}ValueByCoord;

typedef struct VALUES{
    Date date;
    ValueByCoord* values_by_coord;
}Values;

/*
struct ParmTable {
	/* char *name, *comment; */
	//const char *name, *comment;
/*
};

extern const struct ParmTable parm_table_ncep_opn[256];
extern const struct ParmTable parm_table_ncep_reanal[256];
extern const struct ParmTable parm_table_nceptab_128[256];
extern const struct ParmTable parm_table_nceptab_129[256];
extern const struct ParmTable parm_table_nceptab_130[256];
extern const struct ParmTable parm_table_nceptab_131[256];
extern const struct ParmTable parm_table_nceptab_133[256];
extern const struct ParmTable parm_table_nceptab_140[256];
extern const struct ParmTable parm_table_nceptab_141[256];
extern const struct ParmTable parm_table_mdl_nceptab[256];

extern const struct ParmTable parm_table_ecmwf_128[256];
extern const struct ParmTable parm_table_ecmwf_129[256];
extern const struct ParmTable parm_table_ecmwf_130[256];
extern const struct ParmTable parm_table_ecmwf_131[256];
extern const struct ParmTable parm_table_ecmwf_132[256];
extern const struct ParmTable parm_table_ecmwf_133[256];
extern const struct ParmTable parm_table_ecmwf_140[256];
extern const struct ParmTable parm_table_ecmwf_150[256];
extern const struct ParmTable parm_table_ecmwf_151[256];
extern const struct ParmTable parm_table_ecmwf_160[256];
extern const struct ParmTable parm_table_ecmwf_162[256];
extern const struct ParmTable parm_table_ecmwf_170[256];
extern const struct ParmTable parm_table_ecmwf_171[256];
extern const struct ParmTable parm_table_ecmwf_172[256];
extern const struct ParmTable parm_table_ecmwf_173[256];
extern const struct ParmTable parm_table_ecmwf_174[256];
extern const struct ParmTable parm_table_ecmwf_180[256];
extern const struct ParmTable parm_table_ecmwf_190[256];
extern const struct ParmTable parm_table_ecmwf_200[256];
extern const struct ParmTable parm_table_ecmwf_210[256];
extern const struct ParmTable parm_table_ecmwf_211[256];
extern const struct ParmTable parm_table_ecmwf_228[256];
extern const struct ParmTable parm_table_dwd_002[256];
extern const struct ParmTable parm_table_dwd_201[256];
extern const struct ParmTable parm_table_dwd_202[256];
extern const struct ParmTable parm_table_dwd_203[256];
extern const struct ParmTable parm_table_dwd_204[256];
extern const struct ParmTable parm_table_dwd_205[256];
extern const struct ParmTable parm_table_cptec_254[256];
extern const struct ParmTable parm_table_jra55_200[256];
*/





