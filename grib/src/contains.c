#include "contains.h"
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

bool contains(const char* from,long int* count, long int* pos, Date* date ,Coord* coord){
    unsigned char *buffer;
    float *array;
    double temp;
    int i, nx, ny;
    long int len_grib, nxny, buffer_size, n_dump;
    unsigned char *msg, *pds, *gds, *bms, *bds, *pointer, *end_msg;
    FILE *input, *dump_file = NULL;
    GridData grid_ = GridData();
    Date date_;
    char line[2000];
    enum {none, dwd, simple} header = simple;

    long int dump = -1;
    int verbose = 0, append = 0, v_time = 0, year_4 = 0, output_PDS_GDS = 0;
	int table_mode = 0;
    char open_parm[3];
    int return_code = 0;
	char* fmt;

	if ((input = fopen(from,"rb")) == NULL) {
        //fprintf(stderr,"could not open file: %s\n", from);
        exit(7);
    }

    if ((buffer = (unsigned char *) malloc(BUFF_ALLOC0)) == NULL) {
	fprintf(stderr,"not enough memory\n");
    }
    buffer_size = BUFF_ALLOC0;

    /* skip dump - 1 records */
    for (i = 1; i < dump; i++) {
	msg = seek_grib(input, pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
	    fprintf(stderr, "ran out of data or bad file\n");
	    exit(8);
	}
	*pos += len_grib;
    }
    if (dump > 0) *count += dump - 1;
    n_dump = 0;

    for (;;) {
fail:
	msg = seek_grib(input, pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
	    // if (mode == INVENTORY || mode == DUMP_ALL) break;
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
        if (read_grib(input, *pos, len_grib, buffer) == 0) {
                fprintf(stderr,"error, could not read to end of record %ld\n",*count);
                exit(8);
	}

	/* parse grib message */

	msg = buffer;
	end_msg = msg + len_grib - 1;

	/* check if last 4 bytes are '7777' */

//	simple check is for last 4 bytes == '7777'
//    	better check to see if pointers don't go past end_msg
//      if (end_msg[0] != 0x37 || end_msg[-1] != 0x37 || end_msg[-2] != 0x37 || end_msg[-3] != 0x37) {
//	    fprintf(stderr,"Skipping illegal grib1 message: error expected ending 7777\n");
//	    *pos++;
//	    goto fail;
//	}

	if (msg + 8 + 27 > end_msg) {
	    *pos++;
	    goto fail;
	}

        pds = (msg + 8);
        pointer = pds + PDS_LEN(pds);

	if (pointer > end_msg) {
	    *pos++;
	    goto fail;
	}

        if (PDS_HAS_GDS(pds)) {
            gds = pointer;
            pointer += GDS_LEN(gds);
	    if (pointer > end_msg) {
	        *pos++;
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
	    *pos++;
	    goto fail;
	}

        bds = pointer;
        pointer += BDS_LEN(bds);

	if (pointer-msg+4 != len_grib) {
	    fprintf(stderr,"Len of grib message is inconsistent.\n");
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
	    exit(8);
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
	if (gds && ! GDS_Harmonic(gds)) {
	/* this grib check only works for simple packing */
	/* turn off if harmonic */
	    if (BDS_NumBits(bds) != 0) {
	        i = BDS_NValues(bds);
	        if (bms != NULL) {
	            i += missing_points(BMS_bitmap(bms),nxny);
	        }
	        if (i != nxny) {
		    return_code = 15;
		    nxny = nx = i;
		    ny = 1;
	        }
	    }
 
        }
#endif
        temp = int_power(10.0, - PDS_DecimalScale(pds));
        grid_.bound.y2 = 0.001*GDS_LatLon_La1(gds);
        grid_.bound.y1 = 0.001*GDS_LatLon_La2(gds);
        grid_.dy = 0.001*GDS_LatLon_dy(gds);
        grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
        grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
        grid_.dx = 0.001*GDS_LatLon_dx(gds);
        grid_.nx = nx;
        grid_.ny = ny;
        grid_.nxny = nxny;
        
        if(coord && !point_in_rect(&grid_.bound,*coord)){
            *pos += len_grib;
            *count++;
            return false;
        }
        
        date_.year = PDS_Year4(pds);
        date_.month = PDS_Month(pds);
        date_.day  = PDS_Day(pds);
        date_.hour = PDS_Hour(pds);
                
        //if everyone is defined
        if(date && date_.year!=date->year){
            *pos += len_grib;
            *count++;
            return false;
        }
        if(date && date_.month!=date->month){
            *pos += len_grib;
            *count++;
            return false;
        }
        if(date && date_.day!=date->day){
            *pos += len_grib;
            *count++;
            return false;
        }
        if(date && date_.hour!=date->hour){
            *pos += len_grib;
            *count++;
            return false;
        }
        }
    n_dump++;
    fflush(stdout);
    return false;
}

const char* get_file_error_text(ErrorCodeData err){
    return err_code_data[(uint)err];
}
    
Result get_from_pos(const char* from,long int* count,long unsigned* pos){
    Result result;
    unsigned char *buffer;
    float *array;
    double temp;
    int i, nx, ny;
    long int len_grib, nxny, buffer_size, n_dump;
    unsigned char *msg, *pds, *gds, *bms, *bds, *pointer, *end_msg;
    FILE *input, *dump_file = NULL;
    GridData grid_ = GridData();
    Date date_;
    char line[2000];
    enum {none, dwd, simple} header = simple;

    long int dump = -1;
    int verbose = 0, append = 0, v_time = 0, year_4 = 0, output_PDS_GDS = 0;
	int table_mode = 0;
    char open_parm[3];
    int return_code = 0;
	char* fmt;

	if ((input = fopen(from,"rb")) == NULL) {
        printf("At openning in get_from_pos\n");
        fprintf(stderr,"could not open file: %s\n", from);
        result.code = OPEN_ERROR;
        return result;
    }

    if ((buffer = (unsigned char *) malloc(BUFF_ALLOC0)) == NULL) {
	    fprintf(stderr,"not enough memory\n");
        result.code = MEMORY;
        return result;
    }
    buffer_size = BUFF_ALLOC0;

    /* skip dump - 1 records */
    for (i = 1; i < dump; i++) {
	msg = seek_grib(input, pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
	    fprintf(stderr, "ran out of data or bad file\n");
	    result.code = BAD_FILE;
        return result;
	}
	*pos += len_grib;
    }
    if (dump > 0) *count += dump - 1;
    n_dump = 0;

    for(;;){
	msg = seek_grib(input, pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
	    // if (mode == INVENTORY || mode == DUMP_ALL) break;
	    fprintf(stderr,"missing GRIB record(s)\n");
	    result.code = MISS_GRIB_REC;
        return result;
	}

        /* read all whole grib record */
        if (len_grib + msg - buffer > buffer_size) {
            buffer_size = len_grib + msg - buffer + 1000;
            buffer = (unsigned char *) realloc((void *) buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr,"ran out of memory\n");
                result.code = RUN_OUT;
                return result;
            }
        }
        if (read_grib(input, *pos, len_grib, buffer) == 0) {
                fprintf(stderr,"error, could not read to end of record %ld\n",*count);
                result.code = READ_END_ERR;
                return result;
	}

	/* parse grib message */

	msg = buffer;
	end_msg = msg + len_grib - 1;

	/* check if last 4 bytes are '7777' */

//	simple check is for last 4 bytes == '7777'
//    	better check to see if pointers don't go past end_msg
//      if (end_msg[0] != 0x37 || end_msg[-1] != 0x37 || end_msg[-2] != 0x37 || end_msg[-3] != 0x37) {
//	    fprintf(stderr,"Skipping illegal grib1 message: error expected ending 7777\n");
//	    *pos++;
//	    goto fail;
//	}

	if (msg + 8 + 27 > end_msg) {
	    *pos++;
	    exit(8);
	}

        pds = (msg + 8);
        pointer = pds + PDS_LEN(pds);

	if (pointer > end_msg) {
	    *pos++;
        result.code = NONE_ERR;
        return result;
	}

        if (PDS_HAS_GDS(pds)) {
            gds = pointer;
            pointer += GDS_LEN(gds);
	    if (pointer > end_msg) {
	        *pos++;
	        result.code = NONE_ERR;
            return result;  
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
	    *pos++;
	    result.code = NONE_ERR;
        return result; 
	}

        bds = pointer;
        pointer += BDS_LEN(bds);

	if (pointer-msg+4 != len_grib) {
	    fprintf(stderr,"Len of grib message is inconsistent.\n");
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
        free(msg);
	    result.code = MISS_END_SECTION;
        return result; 
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
	if (gds && ! GDS_Harmonic(gds)) {
	/* this grib check only works for simple packing */
	/* turn off if harmonic */
	    if (BDS_NumBits(bds) != 0) {
	        i = BDS_NValues(bds);
	        if (bms != NULL) {
	            i += missing_points(BMS_bitmap(bms),nxny);
	        }
	        if (i != nxny) {
		    return_code = 15;
		    nxny = nx = i;
		    ny = 1;
	        }
	    }
 
        }
#endif
        temp = int_power(10.0, - PDS_DecimalScale(pds));
        grid_.bound.y2 = 0.001*GDS_LatLon_La1(gds);
        grid_.bound.y1 = 0.001*GDS_LatLon_La2(gds);
        grid_.dy = 0.001*GDS_LatLon_dy(gds);
        grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
        grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
        grid_.dx = 0.001*GDS_LatLon_dx(gds);
        grid_.nx = nx;
        grid_.ny = ny;
        grid_.nxny = nxny;
        
        *pos += len_grib;
        *count++;
        result.data.bound = grid_.bound;
        
        date_.year = PDS_Year4(pds);
        date_.month = PDS_Month(pds);
        date_.day  = PDS_Day(pds);
        date_.hour = PDS_Hour(pds);
                
        //if everyone is defined
        result.data.date = date_;
        free(msg);
        fclose(input);
        fflush(stdout);
        result.code = NONE_ERR;
    }
        return result; 
}