#pragma once
#include <stdbool.h>

extern struct RECT{
    double x1;
    double x2;
    double y1;
    double y2;
}Rect_s;

#define RECT(...) ((Rect_s) { (.x1 = -1), (.x2 = -1), (.y1 = -1), (.y2 = -1), ##__VA_ARGS__ })

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

extern bool intersect_rect(struct RECT rect1, struct RECT rect2);