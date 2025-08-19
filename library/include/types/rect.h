#pragma once
#include "types/coord.h"

// #define RECT_TYPE
// #define UNDEF_RECT_VALUE -99999999
// STRUCT_BEG(Rect){
//     float x1 DEF_STRUCT_VAL(UNDEF_RECT_VALUE)
//     float x2 DEF_STRUCT_VAL(-UNDEF_RECT_VALUE)
//     float y1 DEF_STRUCT_VAL(UNDEF_RECT_VALUE)
//     float y2 DEF_STRUCT_VAL(-UNDEF_RECT_VALUE)
// }
// STRUCT_END(Rect)

// #ifndef __cplusplus
// #define Rect(...) ((Rect) { .x1 = UNDEF_RECT_VALUE, .x2 = -UNDEF_RECT_VALUE, .y1 = UNDEF_RECT_VALUE, .y2 = -UNDEF_RECT_VALUE, ##__VA_ARGS__ })
// #endif

// extern bool rect_equal(const Rect* lhs,const Rect* rhs);
// extern bool intersect_rect(const Rect* rect1, const Rect* rect2);
// extern Rect intersection_rect(const Rect* rect1, const Rect* rect2);
// extern bool point_in_rect(const Rect* rect, const Coord point);
// extern bool correct_rect(Rect* rect);
// extern bool is_correct_rect(const Rect* rect);
// extern Rect merge_rect(const Rect* r_1,const Rect* r_2);

// template<>
// struct std::hash<Rect>{
//     size_t operator()(const Rect& rect) const noexcept{
//         return ((size_t)rect.x1<<48)+((size_t)rect.y1<<32)+((size_t)rect.x2<<16)+(size_t)rect.y2;
//     }
//     size_t operator[](const Rect& rect){
//         return ((size_t)rect.x1<<48)+((size_t)rect.y1<<32)+((size_t)rect.x2<<16)+(size_t)rect.y2;
//     }
// };
// bool operator==(const Rect& lhs,const Rect& rhs);