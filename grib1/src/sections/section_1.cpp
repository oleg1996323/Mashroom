#include "sections/section_1.h"
#include "stdio.h"
#include "stdlib.h"

void PDStimes(int time_range, int p1, int p2, int time_unit) {

	const char *unit;
	enum {anal, fcst, unknown} type;
	int fcst_len = 0;

	if (time_unit >= 0 && time_unit <= sizeof(units)/sizeof(char *))
             unit = units[time_unit];
	else unit = "";

        /* change x3/x6/x12 to hours */

        if (time_unit == HOURS3) {
	    p1 *= 3; p2 *= 3;
	    time_unit = HOUR;
        }
        else if (time_unit == HOURS6) {
	    p1 *= 6; p2 *= 6;
	    time_unit = HOUR;
        }
        else if (time_unit == HOURS12) {
	    p1 *= 12; p2 *= 12;
	    time_unit = HOUR;
        }
	else if (time_unit == MINUTES30) {
	    p1 *= 30; p2 *= 30;
	    time_unit = MINUTE;
        }
	else if (time_unit == MINUTES15) {
	    p1 *= 15; p2 *= 15;
	    time_unit = MINUTE;
        }
	/* turn off 5/13/2010 
	if (time_unit == MINUTE && p1 % 60 == 0 && p2 % 60 == 0) {
	    p1 /= 60; p2 /= 60;
	    time_unit = HOUR;
        }
	*/

	if (time_unit >= 0 && time_unit <= sizeof(units)/sizeof(char *))
             unit = units[time_unit];
	else unit = "";

	/* figure out if analysis or forecast */
	/* in GRIB, there is a difference between init and uninit analyses */
	/* not case at NMC .. no longer run initialization */
	/* ignore diff between init an uninit analyses */

	switch (time_range) {

	case 0:
	case 1:
	case 113:
	case 114:
	case 118:
		if (p1 == 0) type = anal;
		else {
			type = fcst;
			fcst_len = p1;
		}
		break;
	case 10: /* way NMC uses it, should be unknown? */
		type = fcst;
		fcst_len = p1*256 + p2;
		if (fcst_len == 0) type = anal;
		break;

	case 51:
		type = unknown;
		break;
	case 123:
	case 124:
		type = anal;
		break;

	case 135:
		type = anal;
		break;

	default: type = unknown;
		break;
	}

	/* ----------------------------------------------- */

	if (type == anal) printf("anl:");
	else if (type == fcst) printf("%d%s fcst:",fcst_len,unit);


	if (time_range == 123 || time_range == 124) {
		if (p1 != 0) printf("start@%d%s:",p1,unit);
	}


	/* print time range */


	switch (time_range) {

	case 0:
	case 1:
	case 10:
		break;
	case 2: printf("valid %d-%d%s:",p1,p2,unit);
		break;
	case 3: printf("%d-%d%s ave:",p1,p2,unit);
		break;
	case 4: printf("%d-%d%s acc:",p1,p2,unit);
		break;
	case 5: printf("%d-%d%s diff:",p1,p2,unit);
		break;
        case 6: printf("-%d to -%d %s ave:", p1,p2,unit);
                break;
        case 7: printf("-%d to %d %s ave:", p1,p2,unit);
                break;
	case 11: if (p1 > 0) {
		    printf("init fcst %d%s:",p1,unit);
		}
		else {
	            printf("time?:");
		}
		break;
	case 13: printf("nudge ana %d%s:",p1,unit);
		break;
	case 14: printf("rel. fcst %d%s:",p1,unit);
		break;
	case 51: if (p1 == 0) {
		    /* printf("clim %d%s:",p2,unit); */
		    printf("0-%d%s product:ave@1yr:",p2,unit);
		}
		else if (p1 == 1) {
		    /* printf("clim (diurnal) %d%s:",p2,unit); */
		    printf("0-%d%s product:same-hour,ave@1yr:",p2,unit);
		}
		else {
		    printf("clim? p1=%d? %d%s?:",p1,p2,unit);
		}
		break;
	case 113:
	case 123:
		printf("ave@%d%s:",p2,unit);
		break;
	case 114:
	case 124:
		printf("acc@%d%s:",p2,unit);
		break;
	case 115:
		printf("ave of fcst:%d to %d%s:",p1,p2,unit);
		break;
	case 116:
		printf("acc of fcst:%d to %d%s:",p1,p2,unit);
		break;
	case 118: 
		printf("var@%d%s:",p2,unit);
		break;
	case 128:
		printf("%d-%d%s fcst acc:ave@24hr:", p1, p2, unit);
		break;
	case 129:
		printf("%d-%d%s fcst acc:ave@%d%s:", p1, p2, unit, p2-p1,unit);
		break;
	case 130:
		printf("%d-%d%s fcst ave:ave@24hr:", p1, p2, unit);
		break;
	case 131:
		printf("%d-%d%s fcst ave:ave@%d%s:", p1, p2, unit,p2-p1,unit);
		break;
		/* for CFS */
	case 132:
		printf("%d-%d%s anl:ave@1yr:", p1, p2, unit);
		break;
	case 133:
		printf("%d-%d%s fcst:ave@1yr:", p1, p2, unit);
		break;
	case 134:
		printf("%d-%d%s fcst-anl:rms@1yr:", p1, p2, unit);
		break;
	case 135:
		printf("%d-%d%s fcst-fcst_mean:rms@1yr:", p1, p2, unit);
		break;
	case 136:
		printf("%d-%d%s anl-anl_mean:rms@1yr:", p1, p2, unit);
		break;
	case 137:
		printf("%d-%d%s fcst acc:ave@6hr:", p1, p2, unit);
		break;
	case 138:
		printf("%d-%d%s fcst ave:ave@6hr:", p1, p2, unit);
		break;
	case 139:
		printf("%d-%d%s fcst acc:ave@12hr:", p1, p2, unit);
		break;
	case 140:
		printf("%d-%d%s fcst ave:ave@12hr:", p1, p2, unit);
		break;
		
	default: printf("time?:");
	}
}

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

int verf_time(unsigned char *pds, int *year, int *month, int *day, int *hour) {
    int tr, dtime, unit;

    *year = PDS_Year4(pds);
    *month = PDS_Month(pds);
    *day  = PDS_Day(pds);
    *hour = PDS_Hour(pds);

    /* find time increment */

    dtime = PDS_P1(pds);
    tr = PDS_TimeRange(pds);
    unit = PDS_ForecastTimeUnit(pds);

    if (tr == 10) dtime = PDS_P1(pds) * 256 + PDS_P2(pds);
    if (tr > 1 && tr < 6 ) dtime = PDS_P2(pds);
    if (tr == 6 || tr == 7) dtime = - PDS_P1(pds);

    if (dtime == 0) return 0;

    return add_time(year, month, day, hour, dtime, unit);
}