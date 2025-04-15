#pragma once
#include <stdbool.h>
#include "aux_code/def.h"

STRUCT_BEG(Date)
{
    int year DEF_STRUCT_VAL(-1)
    int month DEF_STRUCT_VAL(-1)
    int day DEF_STRUCT_VAL(-1)
    int hour DEF_STRUCT_VAL(-1)
    #ifdef __cplusplus
    bool operator<(const Date& other) const noexcept;
    bool operator<=(const Date& other) const noexcept;
    bool operator>(const Date& other) const noexcept;
    bool operator>=(const Date& other) const noexcept;
    bool operator==(const Date& other) const noexcept;
    bool operator!=(const Date& other) const noexcept;
    #endif
}
STRUCT_END(Date)

#ifndef __cplusplus
#define Date(...) ((Date){ .year = -1, .month = -1, .day = -1, .hour = -1, ##__VA_ARGS__ })
#endif

#ifdef DEPRECATED

// Функция для проверки високосного года
extern int is_leap_year(int year);
    
// Функция для получения количества дней в месяце
extern int days_in_month(int year, int month);

extern int days_in_year(int year);

// Функция для получения времени в секундах с начала эпохи
extern long long get_epoch_time(const Date* date);
extern long long get_epoch_time_by_args(int year, int month, int day,int hour);
extern Date date_from_epoque(long long int_time);

extern bool date_less(const Date* lhs, const Date* rhs);
extern bool date_equal(const Date* lhs, const Date* rhs);
extern bool date_bigger(const Date* lhs, const Date* rhs);
extern bool date_less_equal(const Date* lhs, const Date* rhs);
extern bool date_bigger_equal(const Date* lhs, const Date* rhs);
extern bool is_correct_date(const Date* date);
extern bool correct_date_interval(Date* from, Date* to);
#include "code_tables/table_4.h"
extern bool is_date_interval(const Date* from, const Date* to, TimeFrame t_unit, uint8_t time_inc);
extern unsigned long hours_to_days(unsigned int hour);
extern Date date_increment(TimeFrame t_unit, uint8_t time_inc);

#endif

bool is_correct_interval(const std::chrono::system_clock::time_point& from,const std::chrono::system_clock::time_point& to){
    return duration_cast<MinTimeRange>(to-from).count()<0;
}