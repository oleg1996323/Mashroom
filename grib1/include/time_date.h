#pragma once

/* time units */

#define MINUTE  0
#define HOUR    1
#define DAY     2
#define MONTH   3
#define YEAR    4
#define DECADE  5
#define NORMAL  6
#define CENTURY 7
#define HOURS3  10
#define HOURS6  11
#define HOURS12  12
#define MINUTES15 13
#define MINUTES30 14
#define SECOND  254

int add_time(int *year, int *month, int *day, int *hour, int dtime, int unit);