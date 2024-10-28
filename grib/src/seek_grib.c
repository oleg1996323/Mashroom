#include "ecmwf_hack.h"
#include "seek_grib.h"
#include "def.h"

unsigned char *seek_grib(FILE *file, unsigned long *pos, long *len_grib, 
        unsigned char *buffer, unsigned int buf_len) {

    int i, len;
    long length_grib;
    static int warn_grib2 = 0;
    clearerr(file);
    while ( !feof(file) ) {

        if (fseek(file, *pos, SEEK_SET) == -1) break;
	i = fread(buffer, sizeof (unsigned char), buf_len, file);     
        if (ferror(file)) break;
        len = i - LEN_HEADER_PDS;
     
        for (i = 0; i < len; i++) {
            if (buffer[i] == 'G' && buffer[i+1] == 'R' && buffer[i+2] == 'I'
                && buffer[i+3] == 'B') {
		/* grib edition 1 */
		if (buffer[i+7] == 1) {
                    *pos = i + *pos;
                    *len_grib = length_grib = (buffer[i+4] << 16) + (buffer[i+5] << 8) +
                            buffer[i+6];

		    /* small records don't have ECMWF hack */
		    if ((length_grib & 0x800000) == 0) { ec_large_grib = 0; return (buffer + i); }

		    /* potential for ECMWF hack */
		    ec_large_grib = 1;
		    *len_grib = echack(file, *pos, length_grib);
                    return (buffer+i);
		}

		/* grib edition 2 */
		else if (buffer[i+7] == 2) {
		    if (warn_grib2++ == 0) fprintf(stderr,"grib2 message ignored (use wgrib2)\n");
		}

            }
        }
	*pos = *pos + (buf_len - LEN_HEADER_PDS);
    }

    *len_grib = 0;
    return (unsigned char *) NULL;
}