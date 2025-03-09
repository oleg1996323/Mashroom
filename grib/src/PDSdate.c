#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "PDSdate.h"
#include "PDS.h"
#include "verf.h"

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

// Функция для проверки високосного года
int is_leap_year(int year) {
    if (year % 4 != 0) return 0;
    if (year % 100 != 0) return 1;
    return (year % 400 == 0);
}
    
// Функция для получения количества дней в месяце
int days_in_month(int year, int month) {
    if (month == 2) {
        return is_leap_year(year) ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    return 31;
}

// Функция для получения времени в секундах с начала эпохи
long long get_epoch_time(const Date* date) {
    int year = date->year;
    int month = date->month;
    int day = date->day;
    int hour = date->hour;

    // Проверка корректности даты
    if (!is_correct_date(date))
        return -1;

    // Количество дней с начала эпохи до начала текущего года
    long long days_since_epoch = 0;
    for (int y = 1970; y < year; ++y) {
        days_since_epoch += is_leap_year(y) ? 366 : 365;
    }

    // Количество дней с начала текущего года до начала текущего месяца
    for (int m = 1; m < month; ++m) {
        days_since_epoch += days_in_month(year, m);
    }

    // Количество дней с начала текущего месяца до текущего дня
    days_since_epoch += day - 1;

    // Общее количество секунд
    long long seconds_since_epoch = days_since_epoch * 86400 + hour * 3600;

    return seconds_since_epoch;
}

bool date_less(const Date* lhs, const Date* rhs){
    return get_epoch_time(lhs)<get_epoch_time(rhs);
}
bool date_equal(const Date* lhs, const Date* rhs){
    return get_epoch_time(lhs)==get_epoch_time(rhs);
}
bool date_bigger(const Date* lhs, const Date* rhs){
    return get_epoch_time(lhs)>get_epoch_time(rhs);
}
bool date_less_equal(const Date* lhs, const Date* rhs){
    return get_epoch_time(lhs)<=get_epoch_time(rhs);
}
bool date_bigger_equal(const Date* lhs, const Date* rhs){
    return get_epoch_time(lhs)>=get_epoch_time(rhs);
}
bool is_correct_date(const Date* date){
    if (date->year < 1970 || date->month < 1 || date->month > 12 || date->day < 1 || date->day > days_in_month(date->year, date->month))
        return false;
    return true;
}
bool correct_date_interval(Date* from, Date* to){
    if(!(is_correct_date(from) && is_correct_date(to)))
        return false;
    return get_epoch_time(from)<=get_epoch_time(to);
}