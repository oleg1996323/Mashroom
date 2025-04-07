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
#include "def.h"
#include "message.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

#include "decode_aux.h"


#ifdef __cplusplus
bool check(char ch, ValidCapitalizeFmt& valid){
	switch (ch)
	{
	case 'C':
		if(!valid.COORD){
			valid.COORD=true;
			return true;
		}
		break;
	case 'D':
		if(!valid.DAY){
			valid.DAY=true;
			return true;
		}
		break;
	case 'M':
		if(!valid.MONTH){
			valid.MONTH=true;
			return true;
		}
		break;
	case 'H':
		if(!valid.HOUR){
			valid.MONTH=true;
			return true;
		}
		break;
	case 'Y':
		if(!valid.YEAR){
			valid.YEAR=true;
			return true;
		}
		else
		break;
	default:
		return false;
		break;
	}
}
#else
bool check(char ch, ValidCapitalizeFmt* valid){
	switch (ch)
	{
	case 'C':
		if(!valid->COORD){
			valid->COORD=true;
			return true;
		}
		break;
	case 'D':
		if(!valid->DAY){
			valid->DAY=true;
			return true;
		}
		break;
	case 'M':
		if(!valid->MONTH){
			valid->MONTH=true;
			return true;
		}
		break;
	case 'H':
		if(!valid->HOUR){
			valid->MONTH=true;
			return true;
		}
		break;
	case 'Y':
		if(!valid->YEAR){
			valid->YEAR=true;
			return true;
		}
		else
		break;
	default:
		return false;
		break;
	}
}
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

int __capitalize_write__(HGri* grid,
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


namespace fs = std::filesystem;

GribDataInfo capitalize(const fs::path& in,
						const fs::path& root_cap_dir_name,
                        const char* fmt_cap,
                        DataFormat output_type) {
	if(!change_directory(root_cap_dir_name)){
		fprintf(stderr,"Cannot change or create directory");
		exit(1);
	}
    HGrib1 grib;
	GribDataInfo result;
	if(!grib.open_grib(in)){
		result.err = ErrorCodeData::OPEN_ERROR;
		return result;
	}

    for (;;) {
		if ((output_type != GRIB)) {
			/* decode numeric data */
	
			result.add_info(grib.message().section_2_.define_grid(),
							grib.message().section_1_.date(),
							grib.message().section_1_.unit_time_range(),
							grib.message().section_1_.)
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