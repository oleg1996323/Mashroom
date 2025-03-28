#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * find next grib header
 *
 * file = what do you think?
 * pos = initial position to start looking at  ( = 0 for 1st call)
 *       returns with position of next grib header (units=bytes)
 * len_grib = length of the grib record (bytes)
 * buffer[buf_len] = buffer for reading/writing
 *
 * returns (char *) to start of GRIB header+PDS
 *         NULL if not found
 *
 * adapted from SKGB (Mark Iredell)
 *
 * v1.1 9/94 Wesley Ebisuzaki
 * v1.2 3/96 Wesley Ebisuzaki handles short records at end of file
 * v1.3 8/96 Wesley Ebisuzaki increase NTRY from 3 to 100 for the folks
 *      at Automation decided a 21 byte WMO bulletin header wasn't long 
 *      enough and decided to go to an 8K header.  
 * v1.4 11/10/2001 D. Haalman, looks at entire file, does not try
 *      to read past EOF
 *      3/8/2010 echack added by Brian Doty
 * v1.5 5/2011 changes for ECMWF who have grib1+grib2 files, scan entire file
 */

#ifndef __cplusplus
unsigned char *seek_grib(FILE *file, unsigned long *pos, long *len_grib, 
        unsigned char *buffer, unsigned int buf_len);

        unsigned char *seek_grib(FILE *file, unsigned long *pos, long *len_grib, 
                unsigned char *buffer, unsigned int buf_len) {
        
            int i, len;
            long length_grib;
            static int warn_grib2 = 0;
            clearerr(file);
            while ( !feof(file) ) {
        
                if (fseek(file, *pos, SEEK_SET) == -1) break;
                    i = fread(buffer, sizeof (unsigned char), buf_len, file);
                if(i==0){
                    break;
                }
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
#else
#include <fstream>
#include <vector>
std::vector<uint8_t>& seek_grib(std::ifstream& file, unsigned long& pos, long& len_grib,std::vector<uint8_t>& buffer);
#endif