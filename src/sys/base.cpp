#include "base.h"

bool operator==(const Rect& lhs,const Rect& rhs){
    return rect_equal(&lhs,&rhs);
}
bool operator<(const Date& lhs,const Date& rhs){
    return ::date_less(&lhs,&rhs);
}
bool operator<=(const Date& lhs,const Date& rhs){
    return ::date_less_equal(&lhs,&rhs);
}
bool operator==(const Date& lhs,const Date& rhs){
    return ::date_equal(&lhs,&rhs);
}
bool operator>(const Date& lhs,const Date& rhs){
    return ::date_bigger(&lhs,&rhs);
}
bool operator>=(const Date& lhs,const Date& rhs){
    return ::date_bigger_equal(&lhs,&rhs);
}