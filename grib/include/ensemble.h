#pragma once

/*
 * ensemble.c   v0.1 wesley ebisuzaki
 *
 * prints ensemble meta-data
 *
 * only for NCEP and ECMWF
 *
 * output format:
 *
 *       ECMWF
 *  ens=n/N:       n:  0=ctl, +/-ve
 *                 N:  total number of members
 *
 *       NCEP
 *  ens=n/type:    n:  0=ctl, +/-ve, CLUST, PROD/
 *                 type: Mn, WtdMn, SDev, NSDev
 *
 * updated 8/06 w. ebisuzaki
 */

void ensemble(unsigned char *pds, int mode);