#pragma once
#include <type_traits>
#include "interpolation.h"

template<typename T_Y1, typename T_Y2, typename T_X1, typename T_X2, typename T_X_VAL>
std::common_type_t<std::decay_t<T_Y2>,std::decay_t<T_Y1>> lin_interp_between(T_Y1&& y_1, T_X1&& x_1, T_Y2&& y_2, T_X2&& x_2, T_X_VAL&& x_value){
    typename std::common_type_t<T_Y1,T_Y2> Func_R_T;
    if(x_value!=x_1){
        auto col_diff = y_2-y_1;
        return (x_value - x_1)/(x_2 - x_1)*(y_2-y_1)+y_1;
    }
    else return y_1;
}