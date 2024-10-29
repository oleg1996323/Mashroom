#pragma once
#include <stdbool.h>

typedef struct{
    double x1;
    double x2;
    double y1;
    double y2;
}Rect;

#define UNDEF_RECT_VALUE -99999999
#define Rect(...) ((Rect) { .x1 = UNDEF_RECT_VALUE, .x2 = -UNDEF_RECT_VALUE, .y1 = UNDEF_RECT_VALUE, .y2 = -UNDEF_RECT_VALUE, ##__VA_ARGS__ })

/*
 * w. ebisuzaki
 *
 *  return x**y
 *
 *
 *  input: double x
 *	   int y
 */

double int_power(double x, int y);

/* ibm2flt       wesley ebisuzaki
 *
 * v1.1 .. faster
 * v1.1 .. if mant == 0 -> quick return
 *
 */


double ibm2flt(unsigned char *ibm);

/*
 * convert a float to an ieee single precision number v1.1
 * (big endian)
 *                      Wesley Ebisuzaki
 *
 * bugs: doesn't handle subnormal numbers
 * bugs: assumes length of integer >= 25 bits
 */

int flt2ieee(float x, unsigned char *ieee);

extern bool intersect_rect(const Rect* rect1, const Rect* rect2);
extern Rect intersection_rect(const Rect* rect1, const Rect* rect2);