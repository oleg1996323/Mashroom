#pragma once
#include <stdio.h>

/* If the encoded grib record length is long enough, we may have an encoding
   of an even longer record length using the ecmwf hack.  To check for this
   requires getting the length of the binary data section.  To get this requires
   getting the lengths of the various sections before the bds.  To see if those
   sections are there requires checking the flags in the pds.  */

long echack(FILE *file, long pos, long len_grib);