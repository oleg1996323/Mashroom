#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "PDS.h"
#include "BDS.h"
#include "GDS.h"
#include "Parm_Table.h"
#include "write.h"
#include "ensemble.h"
#include "func.h"
#include "print.h"
#include "ecmwf_ext.h"
#include "error_handle.h"
#include "read.h"
#include "seek_grib.h"
#include "levels.h"

#include "decode_aux.h"
#include "PDSdate.h"
#include "capitalize.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif



int __capitalize_write__(GridData* grid,
						const char* root_cap_dir_name,
						Date* date,
						unsigned char *pds,
						const char* fmt, 
						enum DATA_FORMAT d_fmt,
						float *array)
	{
	char cur_path[strlen(root_cap_dir_name)+PATH_MAX];
	strcpy(cur_path,root_cap_dir_name);
	char dump_file_name[51];
	FILE* dump_file = NULL;
	memset(dump_file_name,0,51);
	for(int i = 0; i < grid->ny; ++i){
		for(int j = 0; j< grid->nx; ++j){
			u_int8_t fmt_c=0;
			while(fmt_c<strlen(fmt)){
				switch (fmt[fmt_c])
				{
				case 'C':{
					char str[40];
					memset(&str,0,40);
					sprintf(str, "lat%.2f_lon%.2f", grid->bound.y1 - grid->dy*(double)i,grid->bound.x1 + grid->dx*(double)j);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
						//strcpy(cur_path,strcat(strcat(strcat(strcat(cur_path,".omdb"),k5toa(pds)),"/"),str));
						//printf("Try to open dump file: %s",cur_path);	
						dump_file = fopen(dump_file_name,"a");
						assert(dump_file);
					}
					break;
				}
				case 'D':{
					char str[2];
					memset(&str,0,2);
					sprintf(str, "%.d", date->day);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
						//strcpy(cur_path,strcat(strcat(strcat(strcat(cur_path,".omdb"),k5toa(pds)),"/"),str));
						//printf("Try to open dump file: %s",cur_path);	
						dump_file = fopen(dump_file_name,"a");
						assert(dump_file);
					}
					break;
				}
				case 'M':{
					char str[2];
					memset(&str,0,2);
					sprintf(str, "%.d", date->month);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
						//strcpy(cur_path,strcat(strcat(strcat(strcat(cur_path,".omdb"),k5toa(pds)),"/"),str));
						//printf("Try to open dump file: %s",cur_path);	
						dump_file = fopen(dump_file_name,"a");
						assert(dump_file);
					}
					break;
				}
				case 'H':{
					char str[2];
					memset(&str,0,2);
					sprintf(str, "%.d", date->hour);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
						//strcpy(cur_path,strcat(strcat(strcat(strcat(cur_path,".omdb"),k5toa(pds)),"/"),str));
						//printf("Try to open dump file: %s",cur_path);	
						dump_file = fopen(dump_file_name,"a");
						assert(dump_file);
					}
					break;
				}
				case 'Y':{
					char str[4];
					memset(&str,0,4);
					sprintf(str, "%.d", date->year);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
						//strcpy(cur_path,strcat(strcat(strcat(strcat(cur_path,".omdb"),k5toa(pds)),"/"),str));
						//printf("Try to open dump file: %s",cur_path);	
						dump_file = fopen(dump_file_name,"a");
						assert(dump_file);
					}
					break;
				}
				default:
					fprintf(stderr,"Error reading format");
					exit(1);
					break;
				}
				++fmt_c;
			}
			change_directory(root_cap_dir_name);
			getcwd(cur_path,strlen(cur_path));
			//may be usefull to separate in a unique function for C++ use
			//info can be lost if not added to binary (must be added time/date or coordinate (depend of fmt))
			if(d_fmt==TEXT){
				double cur_lat = grid->bound.y1 + grid->dy*(double)i;
				double cur_lon = grid->bound.x1 + grid->dx*(double)j;
				fprintf(dump_file, "%d %d %d %d %g\n",date->year,date->month,date->day,date->hour,array[i*j]);
			}
			else{
				fwrite((void*)&array[i*j], sizeof(float), 1, dump_file);
			}
			if (ferror(dump_file)) {
				fprintf(stderr,"error writing %s\n",dump_file_name);
				exit(8);
			}
			fclose(dump_file);
			memset(dump_file_name,0,51);
			dump_file = NULL;
		}
	}
	free(array);
	fflush(dump_file);
}

int capitalize(const char* in,
                        const char* root_cap_dir_name,
                        const char* fmt_cap,
                        enum DATA_FORMAT output_type) {
	if(!change_directory(root_cap_dir_name)){
		fprintf(stderr,"Cannot change or create directory");
		exit(1);
	}
    unsigned char *buffer;
    float *array;
    double temp;
    int i, nx, ny;
    long int len_grib, nxny, buffer_size, n_dump, count = 1;
    long unsigned pos = 0;
    unsigned char *msg, *pds, *gds, *bms, *bds, *pointer, *end_msg;
    FILE *input, *dump_file = NULL;
    char line[2000];
    enum {none, dwd, simple} header = simple;

    long int dump = -1;
    int verbose = 0, append = 0, v_time = 0, year_4 = 0, output_PDS_GDS = 0;
	int table_mode = 0;
    char open_parm[3];
    int return_code = 0;
	char* fmt;

	if ((input = fopen(in,"rb")) == NULL) {
        //fprintf(stderr,"could not open file: %s\n", in);
        exit(7);
    }
    if(strlen(fmt_cap)==0){
		fmt = (char*)malloc(strlen(fmt_cap));
		strcpy(fmt,fmt_cap);
		fmt = "CYM";
	}
	else{
		fmt = (char*)malloc(3);
		fmt = "CYM";
	}

    if ((buffer = (unsigned char *) malloc(BUFF_ALLOC0)) == NULL) {
	fprintf(stderr,"not enough memory\n");
    }
    buffer_size = BUFF_ALLOC0;

    /* open output file */
	open_parm[0] = append ? 'a' : 'w'; open_parm[1] = 'b'; open_parm[2] = '\0';
	if (output_type == TEXT) open_parm[1] = '\0';

    /* skip dump - 1 records */
    for (i = 1; i < dump; i++) {
	msg = seek_grib(input, &pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
	    fprintf(stderr, "ran out of data or bad file\n");
	    exit(8);
	}
	pos += len_grib;
    }
    if (dump > 0) count += dump - 1;
    n_dump = 0;

    for (;;) {
fail:
	msg = seek_grib(input, &pos, &len_grib, buffer, MSEEK);
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
//      if (end_msg[0] != 0x37 || end_msg[-1] != 0x37 || end_msg[-2] != 0x37 || end_msg[-3] != 0x37) {
//	    fprintf(stderr,"Skipping illegal grib1 message: error expected ending 7777\n");
//	    pos++;
//	    goto fail;
//	}

	if (msg + 8 + 27 > end_msg) {
	    pos++;
	    goto fail;
	}

        pds = (msg + 8);
        pointer = pds + PDS_LEN(pds);

	if (pointer > end_msg) {
	    pos++;
	    goto fail;
	}

        if (PDS_HAS_GDS(pds)) {
            gds = pointer;
            pointer += GDS_LEN(gds);
	    if (pointer > end_msg) {
	        pos++;
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
	    pos++;
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
		if (output_type == GRIB) {
			if (header == dwd) wrtieee_header((int) len_grib, dump_file);
			fwrite((void *) msg, sizeof(char), len_grib, dump_file);
			if (header == dwd) wrtieee_header((int) len_grib, dump_file);
			n_dump++;
		}

		if ((output_type != GRIB) || verbose > 1) {
			/* decode numeric data */
	
				if ((array = (float *) malloc(sizeof(float) * nxny)) == NULL) {
					fprintf(stderr,"memory problems\n");
					exit(8);
				}

			temp = int_power(10.0, - PDS_DecimalScale(pds));

			BDS_unpack(array, bds, BMS_bitmap(bms), BDS_NumBits(bds), nxny,
				temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));

			if (output_type != GRIB) {
				if(dump_file)
					fclose(dump_file);				
				GridData grid_;
				grid_.bound.y1 = 0.001*GDS_LatLon_La1(gds);
				grid_.bound.y2 = 0.001*GDS_LatLon_La2(gds);
				grid_.dy = 0.001*GDS_LatLon_dy(gds);
				grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
				grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
				grid_.dx = 0.001*GDS_LatLon_dx(gds);
				grid_.nx = nx;
				grid_.ny = ny;
				grid_.nxny = nxny;

				Date date_;
				date_.year = PDS_Year4(pds);
				date_.month = PDS_Month(pds);
				date_.day  = PDS_Day(pds);
				date_.hour = PDS_Hour(pds);

				root_cap_dir_name = root_cap_dir_name;

				/* dump code */
				if (output_type == BINARY) {
					__capitalize_write__(&grid_,root_cap_dir_name,&date_,pds,fmt,BINARY,array);
				}
				else if (output_type == TEXT) {
				/* number of points in grid */
					__capitalize_write__(&grid_,root_cap_dir_name,&date_,pds,fmt,TEXT,array);
				}
				n_dump++;
			}
		}
			fflush(stdout);
			
			pos += len_grib;
			count++;
		}

	if (header == dwd && output_type == GRIB) wrtieee_header(0, dump_file);
    fclose(input);
    return (return_code);
}