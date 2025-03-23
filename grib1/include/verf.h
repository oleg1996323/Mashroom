#pragma once

/*
 * verf_time:
 *
 * this routine returns the "verification" time
 * should have behavior similar to gribmap
 *
 */

int verf_time(unsigned char *pds, int *year, int *month, int *day, int *hour);