#pragma once
#include <cstdint>

constexpr bool is_little_endian() {
    union {
        uint16_t value;
        uint8_t bytes[2];
    } test = {0x0102}; // 0x01 (старший байт), 0x02 (младший байт)
    return test.bytes[0] == 0x02; // Если первый байт = 0x02, то LSB (little-endian)
}
#define _IS_LSB_ is_little_endian();
#define LSB _IS_LSB_

#ifndef __cplusplus
#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  ((a) < (b) ? (b) : (a))
#endif
#endif

#ifdef __cplusplus
#define STRUCT_BEG(x) struct x
#define STRUCT_END(x) ;
#define TEMPLATE_STRUCT_BEG(str_name,T) template<typename T> \
                                        struct str_name
#define TEMPLATE_STRUCT_END(x) ;
#define SPECIFIED_TEMPLATE_STRUCT_BEG(str_name,T) template<> \
                                                struct str_name<T>
#define SPECIFIED_TEMPLATE_STRUCT_END(x) ;     
#define DEF_STRUCT_VAL(val) = val;                                        
#else 
#define STRUCT_BEG(x) typedef struct
#define STRUCT_END(x) x;
#define DEF_STRUCT_VAL(val) ;

#endif

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