#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "capitalize.h"
#include "aux_code/directories.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

typedef enum COORD_DEPEND{
	NOT_LL_DEPEND = 0x00,
	LAT_DEPEND = 1L <<0,
	LON_DEPEND = 1L <<1
}CoordDepend;

int __write_grib__(unsigned char* msg,
					long len_grib,
					const char* root_cap_dir_name,
					GridData* grid,
					Date* date,
					unsigned char* pds,
					const char* fmt
					)
{
	char cur_path[strlen(root_cap_dir_name)+PATH_MAX];
	strcpy(cur_path,root_cap_dir_name);
	char dump_file_name[51];
	FILE* dump_file = NULL;
	memset(dump_file_name,0,51);
	CoordDepend is_lat_lon_dependent = NOT_LL_DEPEND;
	{
		u_int8_t fmt_c=0;
		while(fmt_c<strlen(fmt)){
			switch(fmt[fmt_c]){
				case 'c':
				case 'C':
				case 'l':
				case 'L':
				case 'o':
				case 'O':
					fprintf(stdout,"Not allowed capitalize .grib file by lat/lon. Abort.\n");
					exit(1);
					break;
				default:{
					break;
				}
			}
			if(fmt[fmt_c]=='C' || fmt[fmt_c]=='c' || fmt[fmt_c]=='L' || fmt[fmt_c]=='l' || fmt[fmt_c]=='O' || fmt[fmt_c]=='o'){
				fprintf(stdout,"Not allowed capitalize .grib file by lat/lon. Abort.\n");
				exit(1);
			}
			++fmt_c;
		}
	}
		u_int8_t fmt_c=0;
		while(fmt_c<strlen(fmt)){
			switch (fmt[fmt_c])
			{
			case 'd':
			case 'D':{
				char str[2];
				memset(&str,0,2);
				sprintf(str, "%.d", date->day);
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,k5toa(pds));
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'm':
			case 'M':{
				char str[2];
				memset(&str,0,2);
				sprintf(str, "%.d", date->month);
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,k5toa(pds));
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'h':
			case 'H':{
				char str[2];
				memset(&str,0,2);
				sprintf(str, "%.d", date->hour);
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,k5toa(pds));
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'y':
			case 'Y':{
				char str[4];
				memset(&str,0,4);
				sprintf(str, "%.d", date->year);
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,k5toa(pds));
					strcat(dump_file_name,".grib");						
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
		dump_file = fopen(dump_file_name,"a");
		if(!dump_file){
			fprintf(stdout,"Error openning file %s. Abort.\n",dump_file_name);
			exit(1);
		}
		change_directory(root_cap_dir_name);
		assert(dump_file);
		getcwd(cur_path,strlen(cur_path));
		//may be usefull to separate in a unique function for C++ use
		//info can be lost if not added to binary (must be added time/date or coordinate (depend of fmt))
		wrtieee_header((int) len_grib, dump_file);
		fwrite((void *) msg, sizeof(char), len_grib, dump_file);
		if (ferror(dump_file)) {
			fprintf(stderr,"error writing %s\n",dump_file_name);
			exit(1);
		}
		fclose(dump_file);
		memset(dump_file_name,0,51);
}

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
	CoordDepend is_lat_lon_dependent;
	{
		u_int8_t fmt_c=0;
		while(fmt_c<strlen(fmt)){
			if(fmt[fmt_c]=='C' || fmt[fmt_c]=='c'){
				is_lat_lon_dependent = LAT_DEPEND|LON_DEPEND;
				break;
			}
			else if(fmt[fmt_c]=='L' || fmt[fmt_c]=='l'){
				is_lat_lon_dependent = is_lat_lon_dependent | LAT_DEPEND;
			}
			else if(fmt[fmt_c]=='O' || fmt[fmt_c]=='o'){
				is_lat_lon_dependent = is_lat_lon_dependent | LON_DEPEND;
			}
			++fmt_c;
		}
	}
	for(int i = 0; i < grid->ny; ++i){
		for(int j = 0; j< grid->nx; ++j){
			u_int8_t fmt_c=0;
			while((!dump_file || !(is_lat_lon_dependent&LON_DEPEND) || !(is_lat_lon_dependent&LAT_DEPEND)) && fmt_c<strlen(fmt)){
				switch (fmt[fmt_c])
				{
				case 'c':
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
					}
					break;
				}
				case 'o':
				case 'O':{
					char str[40];
					memset(&str,0,40);
					sprintf(str, "lon%.2f", grid->bound.y1 - grid->dy*(double)i,grid->bound.x1 + grid->dx*(double)j);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
					}
					break;
				}
				case 'l':
				case 'L':{
					char str[40];
					memset(&str,0,40);
					sprintf(str, "lat%.2f", grid->bound.y1 - grid->dy*(double)i,grid->bound.x1 + grid->dx*(double)j);
					strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
					if(fmt_c<strlen(fmt))
						change_directory(cur_path);
					if (fmt_c==strlen(fmt)-1){
						strcat(dump_file_name,k5toa(pds));
						strcat(dump_file_name,".omdb");
					}
					break;
				}
				case 'd':
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
					}
					break;
				}
				case 'm':
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
					}
					break;
				}
				case 'h':
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
					}
					break;
				}
				case 'y':
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
			dump_file = fopen(dump_file_name,"a");
			change_directory(root_cap_dir_name);
			assert(dump_file);
			getcwd(cur_path,strlen(cur_path));
			//may be usefull to separate in a unique function for C++ use
			//info can be lost if not added to binary (must be added time/date or coordinate (depend of fmt))
			if(is_lat_lon_dependent&LON_DEPEND || is_lat_lon_dependent&LAT_DEPEND){
				if(d_fmt==TEXT){
					double cur_lat = grid->bound.y1 + grid->dy*(double)i;
					double cur_lon = grid->bound.x1 + grid->dx*(double)j;
					fprintf(dump_file, "%d %d %d %d %g\n",date->year,date->month,date->day,date->hour,array[i*j]);
				}
				else{
					//add lat_lon write additionally
					fwrite((void*)&array[i*j], sizeof(float), 1, dump_file);
						
				}
				if (ferror(dump_file)) {
					fprintf(stderr,"error writing %s\n",dump_file_name);
					exit(1);
				}
				fclose(dump_file);
				memset(dump_file_name,0,51);
			}
			else break;
		}
		if(!(is_lat_lon_dependent&LON_DEPEND) || !(is_lat_lon_dependent&LAT_DEPEND)){
			break;
		}
	}
	if(!(is_lat_lon_dependent&LON_DEPEND) || !(is_lat_lon_dependent&LAT_DEPEND)){
		fwrite((void*)&date->year, sizeof(date->year), 1, dump_file);
		fwrite((void*)&date->month, sizeof(date->month), 1, dump_file);
		fwrite((void*)&date->day, sizeof(date->day), 1, dump_file);
		fwrite((void*)&grid->bound.x1, sizeof(grid->bound.x1), 1, dump_file);
		fwrite((void*)&grid->bound.x2, sizeof(grid->bound.x2), 1, dump_file);
		fwrite((void*)&grid->bound.y1, sizeof(grid->bound.y1), 1, dump_file);
		fwrite((void*)&grid->bound.y2, sizeof(grid->bound.y2), 1, dump_file);
		fwrite((void*)&grid->nx, sizeof(grid->nx), 1, dump_file);
		fwrite((void*)&grid->ny, sizeof(grid->ny), 1, dump_file);
		//add lat_lon write additionally
		fwrite((void*)array, sizeof(float), grid->nx*grid->ny, dump_file);
		if (ferror(dump_file)) {
			fprintf(stderr,"error writing %s\n",dump_file_name);
			exit(1);
		}
		fclose(dump_file);
		memset(dump_file_name,0,51);
		dump_file = NULL;
	}
	fflush(dump_file);
}
#include "def.h"
CapitalizeData capitalize(const char* in,
                        const char* root_cap_dir_name,
                        const char* fmt_cap,
                        enum DATA_FORMAT output_type) {
	if(!change_directory(root_cap_dir_name)){
		fprintf(stderr,"Cannot change or create directory");
		exit(1);
	}
    unsigned char *buffer = NULL;
    float *array = NULL;
    double temp = 0;
    int i = 0, nx = 0, ny = 0;
	long int len_grib = 0, nxny = 0, buffer_size = 0;
    long unsigned pos = 0;
    unsigned char *msg = NULL, *pds = NULL, *gds = NULL, *bms = NULL, *bds = NULL, *pointer = NULL, *end_msg = NULL;
    FILE *input = NULL;
	CapitalizeData data_info = CapitalizeData();
    long int dump = -1;
	char* fmt = NULL;

	if ((input = fopen(in,"rb")) == NULL) {
        //fprintf(stderr,"could not open file: %s\n", in);
		data_info.err = OPEN_ERROR;
        return data_info;
    }
    if(strlen(fmt_cap)!=0){
		fmt = (char*)malloc(strlen(fmt_cap));
		strcpy(fmt,fmt_cap);
	}
	else{
		fmt = (char*)malloc(3);
		fmt = "YM";
	}

    if ((buffer = (unsigned char *) malloc(BUFF_ALLOC0)) == NULL) {
		fprintf(stderr,"not enough memory\n");
		data_info.err = MEMORY_ERROR;
        return data_info;
    }
    buffer_size = BUFF_ALLOC0;

    for (;;) {
fail:
	msg = seek_grib(input, &pos, &len_grib, buffer, MSEEK);
	if (msg == NULL) {
		if(len_grib==0){
			data_info.err = NONE_ERR;
			return data_info;
		}
	    fprintf(stderr,"missing GRIB record(s)\n");
	    data_info.err = MISS_GRIB_REC;
        return data_info;
	}

        /* read all whole grib record */
        if (len_grib + msg - buffer > buffer_size) {
            buffer_size = len_grib + msg - buffer + 1000;
            buffer = (unsigned char *) realloc((void *) buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr,"ran out of memory\n");
				data_info.err = RUN_OUT;
				return data_info;
            }
        }
        if (read_grib(input, pos, len_grib, buffer) == 0) {
			fprintf(stderr,"error, could not read to end of record %ld\n",data_info.sections);
			data_info.err = READ_END_ERR;
			return data_info;
		}

	/* parse grib message */

	msg = buffer;
	end_msg = msg + len_grib - 1;

	/* check if last 4 bytes are '7777' */

//	simple check is for last 4 bytes == '7777'
//    	better check to see if pointers don't go past end_msg
     if (end_msg[0] != 0x37 || end_msg[-1] != 0x37 || end_msg[-2] != 0x37 || end_msg[-3] != 0x37) {
	    fprintf(stderr,"Skipping illegal grib1 message: error expected ending 7777\n");
	    pos++;
	    goto fail;
	}

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
		Date date_;
		
		grid_.bound.y1 = 0.001*GDS_LatLon_La1(gds);
		grid_.bound.y2 = 0.001*GDS_LatLon_La2(gds);
		grid_.dy = 0.001*GDS_LatLon_dy(gds);
		grid_.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
		grid_.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
		grid_.dx = 0.001*GDS_LatLon_dx(gds);
		grid_.nx = nx;
		grid_.ny = ny;
		grid_.nxny = nxny;
		
		/*
		data_info.grid_data.bound.y1 = 0.001*GDS_LatLon_La1(gds);
		data_info.grid_data.bound.y2 = 0.001*GDS_LatLon_La2(gds);
		data_info.grid_data.dy = 0.001*GDS_LatLon_dy(gds);
		data_info.grid_data.bound.x1 = 0.001*GDS_LatLon_Lo1(gds);
		data_info.grid_data.bound.x2 = 0.001*GDS_LatLon_Lo2(gds);
		data_info.grid_data.dx = 0.001*GDS_LatLon_dx(gds);
		data_info.grid_data.nx = nx;
		data_info.grid_data.ny = ny;
		data_info.grid_data.nxny = nxny;
		*/

		date_.year = PDS_Year4(pds);
		date_.month = PDS_Month(pds);
		date_.day  = PDS_Day(pds);
		date_.hour = PDS_Hour(pds);
		if(!is_correct_date(&data_info.from) || get_epoch_time(&date_)<get_epoch_time(&data_info.from))
			data_info.from=date_;
		if(!is_correct_date(&data_info.to) || get_epoch_time(&date_)>get_epoch_time(&data_info.to))
			data_info.to=date_;

		data_info.grid_data=grid_;
		if ((output_type != GRIB)) {
			/* decode numeric data */
	
			if ((array = (float *) malloc(sizeof(float) * nxny)) == NULL) {
				fprintf(stderr,"memory problems\n");
				exit(8);
			}	
			temp = int_power(10.0, - PDS_DecimalScale(pds));
			BDS_unpack(array, bds, BMS_bitmap(bms), BDS_NumBits(bds), nxny,
				temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));
			/* dump code */
			if (output_type == BINARY) {
				__capitalize_write__(&grid_,root_cap_dir_name,&date_,pds,fmt,BINARY,array);
			}
			else if (output_type == TEXT) {
			/* number of points in grid */
				__capitalize_write__(&grid_,root_cap_dir_name,&date_,pds,fmt,TEXT,array);
			}
			free(array);
			array = NULL;
		}
		else{
			__write_grib__(msg,len_grib,root_cap_dir_name,&grid_,&date_,pds,fmt);
		}
			fflush(stdout);
			
			pos += len_grib;
			data_info.sections++;
	}
    fclose(input);
	free(fmt);
	data_info.err = ErrorCodeData::NONE_ERR;
	return data_info;
}