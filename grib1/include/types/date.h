#pragma once
#include <stdbool.h>
#include "aux_code/def.h"

STRUCT_BEG(Date)
{
    int year DEF_STRUCT_VAL(-1)
    int month DEF_STRUCT_VAL(-1)
    int day DEF_STRUCT_VAL(-1)
    int hour DEF_STRUCT_VAL(-1)
}
STRUCT_END(Date)

#ifndef __cplusplus
#define Date(...) ((Date){ .year = -1, .month = -1, .day = -1, .hour = -1, ##__VA_ARGS__ })
#endif

// Функция для проверки високосного года
extern int is_leap_year(int year);
    
// Функция для получения количества дней в месяце
extern int days_in_month(int year, int month);

// Функция для получения времени в секундах с начала эпохи
extern long long get_epoch_time(const Date* date);

extern bool date_less(const Date* lhs, const Date* rhs);
extern bool date_equal(const Date* lhs, const Date* rhs);
extern bool date_bigger(const Date* lhs, const Date* rhs);
extern bool date_less_equal(const Date* lhs, const Date* rhs);
extern bool date_bigger_equal(const Date* lhs, const Date* rhs);
extern bool is_correct_date(const Date* date);
extern bool correct_date_interval(Date* from, Date* to);