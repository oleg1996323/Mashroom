#include "message.h"

std::ranges::subrange<std::vector<char>::iterator> seek_grib(std::ifstream& file, unsigned long& pos, long& len_grib,std::vector<char>& buffer){
    int i, len;
    long length_grib;
    static int warn_grib2 = 0;
    file.clear();
    while ( !file.eof() ) {
        if(file.seekg(pos,std::ios_base::seekdir::_S_beg).exceptions()) break;
        i = file.readsome(buffer.data(),sizeof(u_int8_t)*buffer.size());
        if(i==0)
            break;
        if (file.fail()) break;
        len = i - LEN_HEADER_PDS;
        
        for (i = 0; i < len; i++) {
            if (buffer.at(i) == 'G' && buffer.at(i+1) == 'R' && buffer.at(i+2) == 'I'
                && buffer.at(i+3) == 'B') {
        /* grib edition 1 */
        if (buffer.at(i+7) == 1) {
                    pos = i + pos;
                    len_grib = length_grib = (buffer.at(i+4) << 16) + (buffer.at(i+5) << 8) +
                            buffer.at(i+6);

            /* small records don't have ECMWF hack */
            if ((length_grib & 0x800000) == 0) { ec_large_grib = 0; return (std::ranges::subrange(buffer.begin() + i,buffer.end())); }

            /* potential for ECMWF hack */
            ec_large_grib = 1;
            len_grib = echack(file, pos, length_grib);
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