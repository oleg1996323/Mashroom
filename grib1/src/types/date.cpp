#include "types/date.h"
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_1.h"

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

long long get_epoch_time_by_args(int year, int month, int day,int hour){
    Date date{year,month,day,hour};
    // Проверка корректности даты
    if (!is_correct_date(&date))
        return -1;

    // Количество дней с начала эпохи до начала текущего года
    long long days_since_epoch = 0;
    for (int y = 1970; y < date.year; ++y) {
        days_since_epoch += is_leap_year(y) ? 366 : 365;
    }

    // Количество дней с начала текущего года до начала текущего месяца
    for (int m = 1; m < date.month; ++m) {
        days_since_epoch += days_in_month(date.year, m);
    }

    // Количество дней с начала текущего месяца до текущего дня
    days_since_epoch += date.day - 1;

    // Общее количество секунд
    long long seconds_since_epoch = days_since_epoch * 86400 + date.hour * 3600;

    return seconds_since_epoch;
}

// Функция для получения времени в секундах с начала эпохи
long long get_epoch_time(const Date* date) {

    // Проверка корректности даты
    if (!is_correct_date(date))
        return -1;

    // Количество дней с начала эпохи до начала текущего года
    long long days_since_epoch = 0;
    for (int y = 1970; y < date->year; ++y) {
        days_since_epoch += is_leap_year(y) ? 366 : 365;
    }

    // Количество дней с начала текущего года до начала текущего месяца
    for (int m = 1; m < date->month; ++m) {
        days_since_epoch += days_in_month(date->year, m);
    }

    // Количество дней с начала текущего месяца до текущего дня
    days_since_epoch += date->day - 1;

    // Общее количество секунд
    long long seconds_since_epoch = days_since_epoch * 86400 + date->hour * 3600;

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
#include <stdexcept>
unsigned long hours_to_days(unsigned int hour){
    return hour/24;
}
Date date_increment(TimeRange t_unit, uint8_t time_inc){
    Date inc{0,0,0,0};
    switch (t_unit)
    {
    case MINUTE:
    case SECOND:
    case HALF_HOUR:
    case QUARTER_HOUR:
        throw std::runtime_error("Still not supported");
        break;
    case HOUR:
        inc.hour = time_inc;
        break;
    case DAY:
        inc.day = time_inc;
        break;
    case MONTH:
        inc.month = time_inc;
        break;
    case YEAR:
        inc.year = time_inc;
        break;
    case HOURS_12:
        inc.hour = time_inc*12;
        break;
    case HOURS_3:
        inc.hour = time_inc*3;
        break;
    case HOURS_6:
        inc.hour = time_inc*6;
    case TimeFrame::DECADE:
        inc.year = time_inc*10;
        break;
    case TimeFrame::CENTURY:
        inc.year = time_inc*100;
        break;
    default:
        throw std::invalid_argument("Invalid time unit");
        break;
    }
    return inc;
}
#ifdef __cplusplus
bool Date::operator<(const Date& other) const noexcept{
    return date_less(this,&other);
}
bool Date::operator<=(const Date& other) const noexcept{
    return date_less_equal(this,&other);
}
bool Date::operator>(const Date& other) const noexcept{
    return date_bigger(this,&other);
}
bool Date::operator>=(const Date& other) const noexcept{
    return date_bigger_equal(this,&other);
}
bool Date::operator==(const Date& other) const noexcept{
    return date_equal(this,&other);
}
bool Date::operator!=(const Date& other) const noexcept{
    return !(*this==other);
}
#endif
