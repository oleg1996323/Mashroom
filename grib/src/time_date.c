#include <stdio.h>
#include "time_date.h"
#include "PDSdate.h"


static int leap(int year) {
    if (year % 4 != 0) return 0;
    if (year % 100 != 0) return 1;
    return (year % 400 == 0);
}

int add_time(int *year, int *month, int *day, int *hour, int dtime, int unit) {
    int y, m, d, h, jday, i, days_in_month;
    y = *year;
    m = *month;
    d = *day;
    h = *hour;
    if (unit == YEAR) {
	*year = y + dtime;
	return 0;
    }
    if (unit == DECADE) {
	*year =  y + (10 * dtime);
	return 0;
    }
    if (unit == CENTURY) {
	*year = y + (100 * dtime);
	return 0;
    }
    if (unit == NORMAL) {
	*year = y + (30 * dtime);
	return 0;
    }
    if (unit == MONTH) {
        if (dtime < 0) {
           i = (-dtime) / 12 + 1;
           y -= i;
           dtime += (i * 12);
        }
	dtime += (m - 1);
	*year =  y = y + (dtime / 12);
	*month = m = 1 + (dtime % 12);

        /* check if date code if valid */
	days_in_month = monthjday[m] - monthjday[m-1];
	if (m == 2 && leap(y)) {
	    days_in_month++;
	}
	if (days_in_month < d) *day = days_in_month;

	return 0;
    }

    if (unit == SECOND) {
	dtime /= 60;
	unit = MINUTE;
    }
    if (unit == MINUTE) {
	dtime /= 60;
	unit = HOUR;
    }

    if (unit == HOURS3) {
        dtime *= 3;
        unit = HOUR;
    }
    else if (unit == HOURS6) {
        dtime *= 6;
        unit = HOUR;
    }
    else if (unit == HOURS12) {
        dtime *= 12;
        unit = HOUR;
    }

    if (unit == HOUR) {
	dtime += h;

        *hour = dtime % 24;
        dtime = dtime / 24;
        if (*hour < 0) {
            *hour += 24;
            dtime--;
        }
        unit = DAY;
    }

    if (unit == DAY) {
	/* set m and day to Jan 0, and readjust dtime */
	jday = d + monthjday[m-1];
	if (leap(y) && m > 2) jday++;
        dtime += jday;

        while (dtime < 1) {
            y--;
	    dtime += 365 + leap(y);
        }

	/* one year chunks */
	while (dtime > 365 + leap(y)) {
	    dtime -= (365 + leap(y));
	    y++;
	}

	/* calculate the month and day */

	if (leap(y) && dtime == FEB29) {
	    m = 2;
	    d = 29;
	}
	else {
	    if (leap(y) && dtime > FEB29) dtime--;
	    for (i = 11; monthjday[i] >= dtime; --i);
	    m = i + 1;
	    d = dtime - monthjday[i];
	}
	*year = y;
	*month = m;
	*day = d;
	return 0;
   }
   fprintf(stderr,"add_time: undefined time unit %d\n", unit);
   return 1;
}