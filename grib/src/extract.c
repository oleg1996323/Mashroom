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

GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos){
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
        fprintf(stderr,"could not open file: %s\n", from);
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
        if(data){
            grid_.bound.y2 = 0.001*GDS_LatLon_La1(gds);
            grid_.bound.y1 = 0.001*GDS_LatLon_La2(gds);
            grid_.dy = 0.001*GDS_LatLon_dy(gds);
            grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
            grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
            grid_.dx = 0.001*GDS_LatLon_dx(gds);
            grid_.nx = nx;
            grid_.ny = ny;
            grid_.nxny = nxny;
            
            if(intersect_rect(&grid_.bound,&data->bound)){
                Rect zone_load = intersection_rect(&grid_.bound,&data->bound);

                date_.year = PDS_Year4(pds);
                date_.month = PDS_Month(pds);
                date_.day  = PDS_Day(pds);
                date_.hour = PDS_Hour(pds);
                
                //if everyone is defined
                if(data->date.year!=-1 && date_.year!=data->date.year){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.month!=-1 && date_.month!=data->date.month){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.day!=-1 && date_.day!=data->date.day){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.hour!=-1 && date_.hour!=data->date.hour){
                    *pos += len_grib;
                    *count++;
                    continue;
                }

                if(!strcmp(data->data_name,k5toa(pds)))
                    printf("%s = %s\n",k5toa(pds),data->data_name);

                if(!data->data_name)
                    fprintf(stdout,"Doesn't exists");

                if(!data->data_name || (data->data_name && !strcmp(data->data_name,k5toa(pds)))){
                    int begin_id_x = ceil((fabs(grid_.bound.x1 - zone_load.x1))/grid_.dx);
                    int begin_id_y = ceil((fabs(grid_.bound.y1 - zone_load.y1))/grid_.dy);
                    int end_id_x = ceil((fabs(grid_.bound.x1 - zone_load.x2))/grid_.dx)+1;
                    int end_id_y = ceil((fabs(grid_.bound.y1 - zone_load.y2))/grid_.dy)+1;

                    if ((array = (float *) malloc(sizeof(float) * nxny)) == NULL) {
                        fprintf(stderr,"memory problems\n");
                        exit(8);
                    }
                    BDS_unpack(array, bds, BMS_bitmap(bms), BDS_NumBits(bds), nxny,
                        temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));
                    
                    if(!(*values = (ValueByCoord*)malloc(sizeof(ValueByCoord)*
                                                    (end_id_x - begin_id_x)*
                                                    (end_id_y - begin_id_y)))){
                        fprintf(stderr,"memory problems\n");
                        exit(1);
                    }
                    else{
                        GridData grid_result;
                        grid_result.nxny = (end_id_x - begin_id_x)*(end_id_y - begin_id_y);
                        grid_result.bound = zone_load;
                        grid_result.nx = (end_id_x - begin_id_x);
                        grid_result.ny = (end_id_y - begin_id_y);
                        grid_result.dx = grid_.dx;
                        grid_result.dy = grid_.dy;
                        for(int y_iter = begin_id_y;y_iter<end_id_y;++y_iter){
                            for(int x_iter = begin_id_x;x_iter<end_id_x;++x_iter){
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lat = 
                                                zone_load.y1+(y_iter-begin_id_y)*grid_.dy;
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lon = 
                                                zone_load.x1+(x_iter-begin_id_x)*grid_.dx;
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].value = 
                                                array[nx*y_iter+x_iter];
                                printf("Lat: %.2f;Lon: %.2f;Value: %.2f\n",
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lat,
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lon,
                                (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].value);
                                if(y_iter==180 || x_iter==648)
                                    printf("Cycle");
                                assert(((*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lat!=0));
                                assert((*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(end_id_y-1 - y_iter)].lon!=0);
                            }
                        }
                        return grid_result;
                    }
                }
                else{
                    *pos += len_grib;
                    *count++;
                    continue;
                }
            }
        }
        n_dump++;
        fflush(stdout);
        
        *pos += len_grib;
        *count++;
    }
    fclose(input);
    return GridData();
}

ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos){
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
        fprintf(stderr,"could not open file: %s\n", from);
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
        if(data){
            grid_.bound.y2 = 0.001*GDS_LatLon_La1(gds);
            grid_.bound.y1 = 0.001*GDS_LatLon_La2(gds);
            grid_.dy = 0.001*GDS_LatLon_dy(gds);
            grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
            grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
            grid_.dx = 0.001*GDS_LatLon_dx(gds);
            grid_.nx = nx;
            grid_.ny = ny;
            grid_.nxny = nxny;
            
            if(intersect_rect(&grid_.bound,&data->bound)){
                Rect zone_load = intersection_rect(&grid_.bound,&data->bound);

                date_.year = PDS_Year4(pds);
                date_.month = PDS_Month(pds);
                date_.day  = PDS_Day(pds);
                date_.hour = PDS_Hour(pds);
                
                //if everyone is defined
                if(data->date.year!=-1 && date_.year!=data->date.year){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.month!=-1 && date_.month!=data->date.month){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.day!=-1 && date_.day!=data->date.day){
                    *pos += len_grib;
                    *count++;
                    continue;
                }
                if(data->date.hour!=-1 && date_.hour!=data->date.hour){
                    *pos += len_grib;
                    *count++;
                    continue;
                }

                if(!strcmp(data->data_name,k5toa(pds)))
                    printf("%s = %s\n",k5toa(pds),data->data_name);

                if(!data->data_name)
                    fprintf(stdout,"Doesn't exists");

                if(!data->data_name || (data->data_name && !strcmp(data->data_name,k5toa(pds)))){
                    int begin_id_x = ceil((fabs(grid_.bound.x1 - zone_load.x1))/grid_.dx);
                    int begin_id_y = ceil((fabs(grid_.bound.y1 - zone_load.y1))/grid_.dy);
                    int end_id_x = ceil((fabs(grid_.bound.x1 - zone_load.x2))/grid_.dx)+1;
                    int end_id_y = ceil((fabs(grid_.bound.y1 - zone_load.y2))/grid_.dy)+1;

                    if ((array = (float *) malloc(sizeof(float) * nxny)) == NULL) {
                        fprintf(stderr,"memory problems\n");
                        exit(8);
                    }
                    BDS_unpack(array, bds, BMS_bitmap(bms), BDS_NumBits(bds), nxny,
                        temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));
                    float* values;
                    if(!(values = (float*)malloc(sizeof(float)*
                                                    (end_id_x - begin_id_x)*
                                                    (end_id_y - begin_id_y)))){
                        fprintf(stderr,"memory problems\n");
                        exit(1);
                    }
                    else{
                        ValuesGrid grid_result;
                        grid_result.nx = (end_id_x - begin_id_x);
                        grid_result.ny = (end_id_y - begin_id_y);
                        grid_result.values = values;
                        for(int y_iter = end_id_y;y_iter>=begin_id_y;--y_iter){
                            for(int x_iter = begin_id_x;x_iter<end_id_x;++x_iter){
                                values[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(y_iter-begin_id_y)] = 
                                                array[nx*y_iter+x_iter];
                                // printf("Lat: %.2f;Lon: %.2f;Value: %.2f\n",
                                // (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(y_iter-begin_id_y)].lat,
                                // (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(y_iter-begin_id_y)].lon,
                                // (*values)[(x_iter-begin_id_x)+(end_id_x - begin_id_x)*(y_iter-begin_id_y)].value);
                                // if(y_iter==180 || x_iter==648)
                                //     printf("Cycle");
                            }
                        }
                        return grid_result;
                    }
                }
                else{
                    *pos += len_grib;
                    *count++;
                    continue;
                }
            }
        }
        n_dump++;
        fflush(stdout);
        
        *pos += len_grib;
        *count++;
    }
    fclose(input);
    return VALUESGRID();
}