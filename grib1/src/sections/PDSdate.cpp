#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "data/PDSdate.h"
#include "sections/section_1.h"
#include "types/date.h"

static int msg_count = 0;
int minute = 0;
int ncep_ens = 0;
int cmc_eq_ncep = 0;
int ec_large_grib = 0,  len_ec_bds;
int PDS_date(unsigned char *pds, int option, int v_time) {

    Date date_;
    int min;

    if (v_time == 0) {
        date_.year = PDS_Year4(pds);
        date_.month = PDS_Month(pds);
        date_.day  = PDS_Day(pds);
        date_.hour = PDS_Hour(pds);
    }
    else {
        if (verf_time(pds, &date_.year, &date_.month, &date_.day, &date_.hour) != 0) {
            if (msg_count++ < 5) fprintf(stderr, "PDS_date: problem\n");
        }
    }
    min =  PDS_Minute(pds);

    switch(option) {
	case 0:
	    printf("%2.2d%2.2d%2.2d%2.2d", date_.year % 100, date_.month, date_.day, date_.hour);
	    if (minute) printf("-%2.2d", min);
	    break;
	case 1:
	    printf("%4.4d%2.2d%2.2d%2.2d", date_.year, date_.month, date_.day, date_.hour);
	    if (minute) printf("-%2.2d", min);
	    break;
	default:
	    fprintf(stderr,"missing code\n");
	    exit(8);
    }
    return 0;
}