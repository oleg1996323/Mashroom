#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "func.h"
#include "sections/PDSdate.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

<<<<<<< HEAD:grib1/include/def.h
typedef enum {
    NONE_ERR,
    DATA_EMPTY,
    OPEN_ERROR,
    READ_POS,
    MEMORY_ERROR,
=======
typedef enum ERROR_CODE_DATA{
    NONE_ERR,
    OPEN_ERROR,
    READ_POS,
    MEMORY,
>>>>>>> 8364990 (temp):grib/include/def.h
    BAD_FILE,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION
}ErrorCodeData;

<<<<<<< HEAD:grib1/include/def.h
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

=======
>>>>>>> 8364990 (temp):grib/include/def.h
typedef enum DATA_FORMAT{
    NONE,
    BINARY,
    TEXT, 
    IEEE, 
    GRIB
}DataFormat;

#define UNDEF_GRID_VALUE -99999999
typedef struct GRID_DATA_S
{
	Rect bound;
	double dy;
	double dx;
	int nx;
	int ny;
	long int nxny;
}GridData;

#define GridData(...) ((GridData){.bound = Rect(), \
                            .dy = UNDEF_GRID_VALUE, \
                            .dx = UNDEF_GRID_VALUE, \
                            .nx = UNDEF_GRID_VALUE, \
                            .ny = UNDEF_GRID_VALUE, \
                            .nxny = UNDEF_GRID_VALUE, \
                            ## __VA_ARGS__})

typedef struct VALUE_BY_COORD{
    float value;
    double lat;
    double lon;
}ValueByCoord;

typedef struct VALUES{
    Date date;
    ValueByCoord* values_by_coord;
}Values;

/* version 1.2 of grib headers  w. ebisuzaki */

#define BMS_LEN(bms)		((bms) == NULL ? 0 : (bms[0]<<16)+(bms[1]<<8)+bms[2])
#define BMS_UnusedBits(bms)	((bms) == NULL ? 0 : bms[3])
#define BMS_StdMap(bms)		((bms) == NULL ? 0 : ((bms[4]<<8) + bms[5]))
#define BMS_bitmap(bms)		((bms) == NULL ? NULL : (bms)+6)
#define BMS_nxny(bms)		((((bms) == NULL) || BMS_StdMap(bms)) \
	? 0 : (BMS_LEN(bms)*8 - 48 - BMS_UnusedBits(bms)))
/* cnames_file.c */

/* search order for parameter names
 *
 * #define P_TABLE_FIRST
 * look at external parameter table first
 *
 * otherwise use builtin NCEP-2 or ECMWF-160 first
 */
/* #define P_TABLE_FIRST */

/* search order for external parameter table
 * 1) environment variable GRIBTAB
 * 2) environment variable gribtab
 * 3) the file 'gribtab' in current directory
 */

struct ParmTable {
	/* char *name, *comment; */
	const char *name, *comment;
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


enum Def_NCEP_Table {rean, opn, rean_nowarn, opn_nowarn};

extern enum Def_NCEP_Table def_ncep_table;



#define VERSION "v1.8.5 (9-2023) Wesley Ebisuzaki\n\t\tDWD-tables 2,201-205 (11-28-2005) Helmut P. Frank\n\t\tspectral: Luis Kornblueh (MPI)"

#define CHECK_GRIB
/* #define DEBUG */

/*
 * wgrib.c is placed into the public domain.  While you could
 * legally do anything you want with the code, telling the world
 * that you wrote it would be uncool.  Selling it would be really
 * uncool.  The code was originally written for NMC/NCAR Reanalysis 
 * and handles most GRIB files except for the ECMWF spectral files.
 * (ECMWF's spectral->grid code are copyrighted and in FORTRAN.)
 * The code, as usual, is not waranteed to be fit for any purpose 
 * what so ever.  However, wgrib is operational NCEP code, so it
 * better work for our files.
 */

/*
 * wgrib.c extract/inventory grib records
 *
 *                              Wesley Ebisuzaki
 *
 * See Changes for update information
 *
 */

/*
 * MSEEK = I/O buffer size for seek_grib
 */

#define MSEEK 1024
#define BUFF_ALLOC0	40000

#ifndef DEF_T62_NCEP_TABLE
#define DEF_T62_NCEP_TABLE	rean
#endif


/* #define LEN_HEADER_PDS (28+42+100) */
#define LEN_HEADER_PDS (28+8)

extern int ec_large_grib;


	


/* cnames.c 				Wesley Ebisuzaki
 *
 * returns strings with either variable name or comment field
 * v1.4 4/98
 * reanalysis can use process 180 and subcenter 0
 *
 * Add DWD tables 2, 201, 202, 203      Helmut P. Frank, DWD, FE13
 *                                      Thu Aug 23 09:28:34 GMT 2001
 * add DWD tables 204, 205              H. Frank, 10-19-2005
 * LAMI => DWD				11/2008 Davide Sacchetti 
 * add JRA-55 table 200
 */



extern int cmc_eq_ncep;

#define START -1

static int user_center = 0, user_subcenter = 0, user_ptable = 0;
typedef enum {filled, not_found, not_checked, no_file, init}STATUS;

extern struct ParmTable parm_table_user[256];

extern int ncep_ens;

/*
 * support for complex packing
 *  determine the number of data points in the BDS
 *  does not handle matrix values
 */

extern int ec_large_grib,  len_ec_bds;

extern enum Def_NCEP_Table def_ncep_table;
extern int minute;
extern int ncep_ens;
extern int cmc_eq_ncep;
extern int ec_large_grib, len_ec_bds;
extern int ec_large_grib;
static STATUS status = init;