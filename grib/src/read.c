#include "read.h"

int read_grib(FILE *file, long pos, long len_grib, unsigned char *buffer) {

    int i;


    if (fseek(file, pos, SEEK_SET) == -1) {
	    return 0;
    }

    i = fread(buffer, sizeof (unsigned char), len_grib, file);
    return (i == len_grib);
}