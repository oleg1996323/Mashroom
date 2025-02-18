#include "extract_cpp.h"
#ifdef __cplusplus
    extern "C"{
        #include <stdio.h>
        #include <stdlib.h>
        #include <stdbool.h>
        #include <float.h>
        #include <assert.h>
        #include <string.h>
        #include <math.h>
        #include "extract.h"
        #include "PDS.h"
        #include "BDS.h"
        #include "GDS.h"
        #include "write.h"
        #include "ensemble.h"
        #include "func.h"
        #include "print.h"
        #include "ecmwf_ext.h"
        #include "error_handle.h"
        #include "read.h"
        #include "seek_grib.h"
        #include "levels.h"
        #include "Parm_Table.h"
        #include "def.h"
        #include <sys/types.h>
        #include <dirent.h>
        #include <unistd.h>
        #include <limits.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <errno.h>
        #include <decode_aux.h>
        #include <png.h>
    }
#endif

namespace cpp{

ReturnVal extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const std::filesystem::path& ffrom,const std::filesystem::path& fto,long int count, long unsigned pos){
    unsigned char *buffer = NULL;
    //float value_res = UNDEFINED;
    ReturnVal result_date;
    int nx = 0, ny = 0;
	long int len_grib = 0, nxny = 0, buffer_size = 0;
    unsigned char *msg = NULL, *pds = NULL, *gds = NULL, *bms = NULL, *bds = NULL, *pointer = NULL, *end_msg = NULL;
    std::ifstream input;
    std::ofstream output;

    long int dump = -1;
	char* fmt = NULL;

    input.open(ffrom,std::ios::binary);
    output.open(fto,std::ios::trunc);

	if (!input.is_open()) {
        //fprintf(stderr,"could not open file: %s\n", in);
        exit(1);
    }
    if(output.is_open()){
        exit(1);
    }

    if ((buffer = (unsigned char *) malloc(BUFF_ALLOC0)) == NULL) {
		fprintf(stderr,"not enough memory\n");
		exit(0);
    }
    buffer_size = BUFF_ALLOC0;

    for (;;) {
fail:
	msg = seek_grib(input, &pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
		if(len_grib==0){
            ReturnVal empty;
            empty.time = Date();
            empty.val = UNDEFINED;
            return UNDEFINED;
        }
	    fprintf(stderr,"missing GRIB record(s)\n");
	    exit(8);
	}

        /* read all whole grib record */
        if (len_grib + msg - buffer > buffer_size) {
            buffer_size = len_grib + msg - buffer + 1000;
            buffer = (unsigned char *) realloc((void *) buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr,"ran out of memory\n");
                exit(8);
            }
        }
        if (read_grib(input, pos, len_grib, buffer) == 0) {
			fprintf(stderr,"error, could not read to end of record %ld\n",count);
			exit(8);
		}

	/* parse grib message */

	msg = buffer;
	end_msg = msg + len_grib - 1;

	/* check if last 4 bytes are '7777' */

//	simple check is for last 4 bytes == '7777'
//    	better check to see if pointers don't go past end_msg
     if (end_msg[0] != 0x37 || end_msg[-1] != 0x37 || end_msg[-2] != 0x37 || end_msg[-3] != 0x37) {
	    fprintf(stderr,"Skipping illegal grib1 message: error expected ending 7777\n");
	    (pos)++;
	    goto fail;
	}

	if (msg + 8 + 27 > end_msg) {
	    (pos)++;
	    goto fail;
	}

        pds = (msg + 8);
        pointer = pds + PDS_LEN(pds);

	if (pointer > end_msg) {
	    (pos)++;
	    goto fail;
	}

	if (PDS_HAS_GDS(pds)) {
		gds = pointer;
		pointer += GDS_LEN(gds);
	if (pointer > end_msg) {
		(pos)++;
		goto fail;
	}
	}
	else {
		gds = NULL;
	}

	if (PDS_HAS_BMS(pds)) {
		bms = pointer;
		pointer += BMS_LEN(bms);
	}
	else {
		bms = NULL;
	}

	if (pointer+10 > end_msg) {
	    (pos)++;
	    goto fail;
	}

	bds = pointer;
	pointer += BDS_LEN(bds);

	if (pointer-msg+4 != len_grib) {
	    fprintf(stderr,"Len of grib message is inconsistent.\n");
		exit(1);
	}

        /* end section - "7777" in ascii */
        if (pointer[0] != 0x37 || pointer[1] != 0x37 ||
            pointer[2] != 0x37 || pointer[3] != 0x37) {
            fprintf(stderr,"\n\n    missing end section\n");
            fprintf(stderr, "%2x %2x %2x %2x\n", pointer[0], pointer[1], 
		pointer[2], pointer[3]);
#ifdef DEBUG
	    printf("ignoring missing end section\n");
#else
	    exit(1);
#endif
        }

	/* figure out size of array */
	if (gds != NULL) {
	    GDS_grid(gds, bds, &nx, &ny, &nxny);
	}
	else if (bms != NULL) {
	    nxny = nx = BMS_nxny(bms);
	    ny = 1;
	}
	else {
	    if (BDS_NumBits(bds) == 0) {
                nxny = nx = 1;
                fprintf(stderr,"Missing GDS, constant record .. cannot "
                    "determine number of data points\n");
	    }
	    else {
	        nxny = nx = BDS_NValues(bds);
	    }
	    ny = 1;
	}
#ifdef CHECK_GRIB
    int i = 0;
	if (gds && ! GDS_Harmonic(gds)) {
	/* this grib check only works for simple packing */
	/* turn off if harmonic */
	    if (BDS_NumBits(bds) != 0) {
	        i = BDS_NValues(bds);
	        if (bms != NULL) {
	            i += missing_points(BMS_bitmap(bms),nxny);
	        }
	        if (i != nxny) {
		    nxny = nx = i;
		    ny = 1;
	        }
	    }
 
        }
#endif
		GridData grid_;
		//ReturnVal result_date;
		
		grid_.bound.y1 = 0.001*GDS_LatLon_La1(gds);
		grid_.bound.y2 = 0.001*GDS_LatLon_La2(gds);
		grid_.dy = 0.001*GDS_LatLon_dy(gds);
		grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
		grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
		grid_.dx = 0.001*GDS_LatLon_dx(gds);
		grid_.nx = nx;
		grid_.ny = ny;
		grid_.nxny = nxny;

		result_date.time.year = PDS_Year4(pds);
        if(dto.year!=-1 && result_date.time.year>dto.year || dfrom.year!=-1 && result_date.time.year<dfrom.year){
            pos += len_grib;
            count++;
            continue;
        }
		result_date.time.month = PDS_Month(pds);
        if(dto.month!=-1 && result_date.time.month>dto.month || dfrom.month!=-1 && result_date.time.month<dfrom.month){
            pos += len_grib;
            count++;
            continue;
        }
		result_date.time.day  = PDS_Day(pds);
        if(dto.day!=-1 && result_date.time.day>dto.day || dfrom.day!=-1 && result_date.time.day<dfrom.day){
            pos += len_grib;
            count++;
            continue;
        }
		result_date.time.hour = PDS_Hour(pds);
        if(dto.hour!=-1 && result_date.time.hour>dto.hour || dfrom.hour!=-1 && result_date.time.hour<dfrom.hour){
            pos += len_grib;
            count++;
            continue;
        }
			/* decode numeric data */
	
        double temp = int_power(10.0, - PDS_DecimalScale(pds));
        int y_offset = (coord.lat_-grid_.bound.y2)/grid_.dy;
        int x_offset = (coord.lon_-grid_.bound.x1)/grid_.dx;
        result_date.val = BDS_unpack_val(bds, BMS_bitmap(bms), BDS_NumBits(bds), (int)(x_offset+grid_.nx*y_offset),
            temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));
        /* dump code */
        fprintf(output,"Series: %s Point: %d (Time: %d/%d/%d %d:00) Val: %f\n",k5toa(pds),(int)(x_offset+grid_.nx*y_offset),
                                result_date.time.year, result_date.time.month,result_date.time.day,result_date.time.hour,result_date.val);
        fflush(output);
        
        (pos) += len_grib;
        (count)++;
	}
    fclose(input);
    fclose(output);
	free(fmt);
    return UNDEFINED;
}

}