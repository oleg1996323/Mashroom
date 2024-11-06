#pragma once
#include <type_traits>
#ifdef __cplusplus
extern "C"{
    #include "interpolation.h"
}
#endif

template<typename T_Y1, typename T_Y2, typename T_X1, typename T_X2, typename T_X_VAL>
std::common_type_t<std::decay_t<T_Y2>,std::decay_t<T_Y1>> lin_interp_between(T_Y1&& y_1, T_X1&& x_1, T_Y2&& y_2, T_X2&& x_2, T_X_VAL&& x_value){
    if(x_value!=x_1){
        auto koef = ((x_value - x_1)/(x_2 - x_1));
        return y_1+(y_2-y_1);
    }
    else return y_1;
}