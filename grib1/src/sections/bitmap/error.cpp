#include <stdlib.h>
#include "sections/bitmap/error_handle.h"

int missing_points(unsigned char *bitmap, int n) {

    int count;
    unsigned int tmp;
    if (bitmap == NULL) return 0;

    count = 0;
    while (n >= 8) {
	tmp = *bitmap++;
	n -= 8;
        count += bitsum[tmp];
    }
    tmp = *bitmap | ((1 << (8 - n)) - 1);
    count += bitsum[tmp];

    return count;
}