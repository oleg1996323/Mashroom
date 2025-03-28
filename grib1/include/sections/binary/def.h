#pragma once
#include "aux_code/ibmtofloat.h"
#include "aux_code/floattoieee.h"
#define BDS_LEN(bds)		(ec_large_grib ? len_ec_bds : ((int) ((bds[0]<<16)+(bds[1]<<8)+bds[2])) )

#define BDS_Flag(bds)		(bds[3])

#define BDS_Grid(bds)		((bds[3] & 128) == 0)
#define BDS_Harmonic(bds)	(bds[3] & 128)

#define BDS_Packing(bds)	((bds[3] & 64) != 0)
#define BDS_SimplePacking(bds)	((bds[3] & 64) == 0)
#define BDS_ComplexPacking(bds)	((bds[3] & 64) != 0)

#define BDS_OriginalType(bds)	((bds[3] & 32) != 0)
#define BDS_OriginalFloat(bds)	((bds[3] & 32) == 0)
#define BDS_OriginalInt(bds)	((bds[3] & 32) != 0)

#define BDS_MoreFlags(bds)      ((bds[3] & 16) != 0)
#define BDS_UnusedBits(bds)	((int) (bds[3] & 15))

#define BDS_BinScale(bds)	INT2(bds[4],bds[5])

#define BDS_RefValue(bds)	(ibm2flt(bds+6))
#define BDS_NumBits(bds)	((int) bds[10])

#define BDS_Harmonic_RefValue(bds) (ibm2flt(bds+11))

#define BDS_DataStart(bds)      ((int) (11 + BDS_MoreFlags(bds)*3))
#define BDS_P1(bds)		(bds[16] * 256 + bds[17])
#define BDS_P2(bds)		(bds[18] * 256 + bds[19])

/* undefined value -- if bitmap */
#define UNDEFINED		9.999e20

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};
static unsigned int map_masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};
static double shift[9] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};

extern void BDS_unpack(float *flt, unsigned char *bds, unsigned char *bitmap,
	int n_bits, int n, double ref, double scale);

extern int BDS_NValues(unsigned char *bds);

extern float BDS_unpack_val(unsigned char *bds, unsigned char *bitmap,
	int n_bits, int n, double ref, double scale);