#pragma once
/*
 * PDS_date.c  v1.2 wesley ebisuzaki
 *
 * prints a string with a date code
 *
 * PDS_date(pds,option, v_time)
 *   options=0  .. 2 digit year
 *   options=1  .. 4 digit year
 *
 *   v_time=0   .. initial time
 *   v_time=1   .. verification time
 *
 * assumption: P1 and P2 are unsigned integers (not clear from doc)
 *
 * v1.2 years that are multiple of 400 are leap years, not 500
 * v1.2.1  make the change to the source code for v1.2
 * v1.2.2  add 3/6/12 hour forecast time units
 * v1.2.3  Jan 31 + 1 month => Feb 31 .. change to Feb 28/29
 */

int PDS_date(unsigned char *pds, int option, int v_time);