#include "func.h"
#include "math.h"
#include "float.h"
#include <stdio.h>
#include <stdbool.h>
#include <error.h>
#include "def.h"

bool intersect_rect(const Rect* rect1, const Rect* rect2) {
	if(rect1 && rect2)
		return !(rect2->x1 > rect1->x2 ||
			rect2->x2 < rect1->x1 || 
			rect2->y1 < rect1->y2||
			rect2->y2 > rect1->y1);
	else {
		fprintf(stderr,"Not defined arguments");
		exit(1);
	}
}

Rect intersection_rect(const Rect* source, const Rect* searched){
	if(source && searched)
		return Rect(.x1 = max(source->x1,searched->x1), 
					.x2 = min(source->x2,searched->x2),
					.y1 = max(source->y1,searched->y1),
					.y2 = min(source->y2,searched->y2));
	else {
		fprintf(stderr,"Not defined arguments");
		exit(1);
	}
}

double int_power(double x, int y) {

	double value;

	if (y < 0) {
		y = -y;
		x = 1.0 / x;
	}
	value = 1.0;

	while (y) {
		if (y & 1) {
			value *= x;
		}
		x = x * x;
		y >>= 1;
	}
	return value;
}

/* ibm2flt       wesley ebisuzaki
 *
 * v1.1 .. faster
 * v1.1 .. if mant == 0 -> quick return
 *
 */

double ibm2flt(unsigned char *ibm) {

	int positive, power;
	unsigned int abspower;
	long int mant;
	double value, exp;

	mant = (ibm[1] << 16) + (ibm[2] << 8) + ibm[3];
        if (mant == 0) return 0.0;

	positive = (ibm[0] & 0x80) == 0;
	power = (int) (ibm[0] & 0x7f) - 64;
	abspower = power > 0 ? power : -power;


	/* calc exp */
	exp = 16.0;
	value = 1.0;
	while (abspower) {
		if (abspower & 1) {
			value *= exp;
		}
		exp = exp * exp;
		abspower >>= 1;
	}

	if (power < 0) value = 1.0 / value;
	value = value * mant / 16777216.0;
	if (positive == 0) value = -value;
	return value;
}

/*
 * convert a float to an ieee single precision number v1.1
 * (big endian)
 *                      Wesley Ebisuzaki
 *
 * bugs: doesn't handle subnormal numbers
 * bugs: assumes length of integer >= 25 bits
 */

int flt2ieee(float x, unsigned char *ieee) {

	int sign, exp;
        unsigned int umant;
	double mant;

	if (x == 0.0) {
		ieee[0] = ieee[1] = ieee[2] = ieee[3] = 0;
		return 0;
	}

	/* sign bit */
	if (x < 0.0) {
		sign = 128;
		x = -x;
	}
	else sign = 0;
	mant = frexp((double) x, &exp);

        /* 2^24 = 16777216 */

	umant = mant * 16777216 + 0.5;
	if (umant >= 16777216) {
            umant = umant / 2;
            exp++;
        }
        /* bit 24 should be a 1 .. not used in ieee format */

	exp = exp - 1 + 127;

	if (exp < 0) {
		/* signed zero */
		ieee[0] = sign;
		ieee[1] = ieee[2] = ieee[3] = 0;
		return 0;
	}
	if (exp > 255) {
		/* signed infinity */
		ieee[0] = sign + 127;
		ieee[1] = 128;
                ieee[2] = ieee[3] = 0;
                return 0;
	}
	/* normal number */

	ieee[0] = sign + (exp >> 1);

        ieee[3] = umant & 255;
        ieee[2] = (umant >> 8) & 255;
        ieee[1] = ((exp & 1) << 7) + ((umant >> 16) & 127);
	return 0;
}