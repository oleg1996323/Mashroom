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

static int msg_count = 0;
extern int minute;

int PDS_date(unsigned char *pds, int option, int v_time);

typedef struct DATE_S{
int year;
int month;
int day;
int hour;
}Date;

#define DATE(...) ((Date){ (.year = -1), (.month = -1), (.day = -1), (.hour = -1), ##__VA_ARGS__ })

#define  FEB29   (31+29)
static int monthjday[13] = {
        0,31,59,90,120,151,181,212,243,273,304,334,365};