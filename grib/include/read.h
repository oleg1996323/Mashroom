#pragma once
#include <stdio.h>

/*
 * read_grib.c
 *
 * reads grib message
 *
 * input: pos, byte position of grib message
 *        len_grib, length of grib message
 * output: *buffer, grib message
 *
 * note: call seek_grib first
 *
 * v1.0 9/94 Wesley Ebisuzaki
 *
 */

int read_grib(FILE *file, long pos, long len_grib, unsigned char *buffer);