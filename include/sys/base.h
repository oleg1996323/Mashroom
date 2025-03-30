#pragma once
#include <unordered_map>
#include <string>
#include <set>
#include "data/PDSdate.h"
#include "types/date.h"
#include "types/rect.h"
#include "tags.h"

template<>
struct std::hash<Rect>{
    size_t operator()(const Rect& rect) const noexcept{
        return ((size_t)rect.x1<<48)+((size_t)rect.y1<<32)+((size_t)rect.x2<<16)+(size_t)rect.y2;
    }
    size_t operator[](const Rect& rect){
        return ((size_t)rect.x1<<48)+((size_t)rect.y1<<32)+((size_t)rect.x2<<16)+(size_t)rect.y2;
    }
};
bool operator==(const Rect& lhs,const Rect& rhs);
bool operator<(const Date& lhs,const Date& rhs);
bool operator<=(const Date& lhs,const Date& rhs);
bool operator==(const Date& lhs,const Date& rhs);
bool operator>(const Date& lhs,const Date& rhs);
bool operator>=(const Date& lhs,const Date& rhs);
template<>
struct std::less<Tag>{
    bool operator()(Tag lhs,Tag rhs) const{
        return lhs<rhs;
    }
};