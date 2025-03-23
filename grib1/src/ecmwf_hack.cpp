#include "ecmwf_hack.h"
#include "def.h"
#include "sections/PDS.h"


long echack(FILE *file, long pos, long len_grib) {

    int gdsflg, bmsflg, center;
    unsigned int pdslen, gdslen, bmslen, bdslen;
    unsigned char buf[8];
    long len;

    len = len_grib;

    /* Get pdslen */

    if (fseek(file, pos+8, SEEK_SET) == -1) return 0;
    if (fread(buf, sizeof (unsigned char), 8, file) != 8) return 0;
    pdslen = __LEN24(buf);

    center = buf[4];

    /* know that NCEP and CMC do not use echack */
    if (center == NCEP || center == CMC) {
	ec_large_grib = 0;
        return len_grib;
    }


    gdsflg = buf[7] & 128;
    bmsflg = buf[7] & 64;

    gdslen=0;
    if (gdsflg) {
        if (fseek(file, pos+8+pdslen, SEEK_SET) == -1) return 0;
        if (fread(buf, sizeof (unsigned char), 3, file) != 3) return 0;
        gdslen = __LEN24(buf);
    }

    /* if there, get length of bms */

    bmslen = 0;
    if (bmsflg) {
       if (fseek(file, pos+8+pdslen+gdslen, SEEK_SET) == -1) return 0;
       if (fread(buf, sizeof (unsigned char), 3, file) != 3) return 0;
       bmslen = __LEN24(buf);
    }

    /* get bds length */

    if (fseek(file, pos+8+pdslen+gdslen+bmslen, SEEK_SET) == -1) return 0;
    if (fread(buf, sizeof (unsigned char), 3, file) != 3) return 0;
    bdslen = __LEN24(buf);

    /* Now we can check if this record is hacked */

    if (bdslen >= 120) {
	/* normal record */
	ec_large_grib = 0;
    }
    else {
        /* ECMWF hack */
        len_grib = (len & 0x7fffff) * 120 - bdslen + 4;
        len_ec_bds = len_grib - (12 + pdslen + gdslen + bmslen);
	ec_large_grib = 1;
    }
    return len_grib;
}