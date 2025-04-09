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

typedef enum{
	NOT_LL_DEPEND = 0x00,
	LAT_DEPEND = 1L <<0,
	LON_DEPEND = 1L <<1
}CoordDepend;

CoordDepend operator|(CoordDepend lhs,CoordDepend rhs){
	return static_cast<CoordDepend>(lhs|rhs);
}

/*
int __write_grib__(const Message& msg,
					const char* root_cap_dir_name,
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
				sprintf(str, "%.d", msg.section_1_.day());
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,msg.section_1_.parameter_name());
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'm':
			case 'M':{
				char str[2];
				memset(&str,0,2);
				sprintf(str, "%.d", msg.section_1_.month());
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,msg.section_1_.parameter_name());
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'h':
			case 'H':{
				char str[2];
				memset(&str,0,2);
				sprintf(str, "%.d", msg.section_1_.hour());
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,msg.section_1_.parameter_name());
					strcat(dump_file_name,".grib");
				}
				break;
			}
			case 'y':
			case 'Y':{
				char str[4];
				memset(&str,0,4);
				sprintf(str, "%.d", msg.section_1_.year());
				strcpy(cur_path,strcat(strcat(cur_path,"/"),str));
				if(fmt_c<strlen(fmt))
					change_directory(cur_path);
				if (fmt_c==strlen(fmt)-1){
					strcat(dump_file_name,msg.section_1_.parameter_name());
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
*/
namespace fs = std::filesystem;
using namespace std::string_literals;

int __capitalize_write__(const Message& msg,
						const GribMsgDataInfo& msg_info,
						const fs::path& root_cap_dir_name,
						const char* fmt, 
						DataFormat d_fmt)
	{
	fs::path cur_path(root_cap_dir_name);
	FILE* dump_file = NULL;
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
	u_int8_t fmt_c=0;
	while(!dump_file && fmt_c<strlen(fmt)){
		switch (fmt[fmt_c])
		{
		case 'd':
		case 'D':{
			std::string str = std::to_string(msg_info.date.day);
			cur_path/=str;
			if(fmt_c<strlen(fmt))
				change_directory(cur_path);
			if (fmt_c==strlen(fmt)-1)
				cur_path/=msg.section_1_.parameter_name()+".grib"s;
			break;
		}
		case 'm':
		case 'M':{
			std::string str = std::to_string(msg_info.date.month);
			cur_path/=str;
			if(fmt_c<strlen(fmt))
				change_directory(cur_path);
			if (fmt_c==strlen(fmt)-1)
				cur_path/=msg.section_1_.parameter_name()+".grib"s;
			break;
		}
		case 'h':
		case 'H':{
			std::string str = std::to_string(msg_info.date.hour);
			cur_path/=str;
			if(fmt_c<strlen(fmt))
				change_directory(cur_path);
			if (fmt_c==strlen(fmt)-1)
				cur_path/=msg.section_1_.parameter_name()+".grib"s;
			break;
			}
		case 'y':
		case 'Y':{
			std::string str = std::to_string(msg_info.date.year);
			cur_path/=str;
			if(fmt_c<strlen(fmt))
				change_directory(cur_path);
			if (fmt_c==strlen(fmt)-1)
				cur_path/=msg.section_1_.parameter_name()+".grib"s;
			break;
		}
		default:
			fprintf(stderr,"Error reading format");
			exit(1);
			break;
		}
		++fmt_c;
	}
		dump_file = fopen(cur_path.c_str(),"a");
		change_directory(root_cap_dir_name);
		assert(dump_file);
		//may be usefull to separate in a unique function for C++ use
		//info can be lost if not added to binary (must be added time/date or coordinate (depend of fmt))
	
	fwrite(msg.section_0_.buf_,sizeof(unsigned char),msg.message_length(),dump_file);
	fclose(dump_file);
	dump_file = NULL;
}


namespace fs = std::filesystem;

GribDataInfo capitalize(const fs::path& in,
						const fs::path& root_cap_dir_name,
                        const char* fmt_cap,
                        DataFormat output_type) {
	if(!fs::exists(root_cap_dir_name))
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

    do{
		GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
									std::move(grib.message().section_1_.date()),
									grib.message().section_1_.IndicatorOfParameter(),
									grib.message().section_1_.unit_time_range(),
									grib.message().section_1_.center(),
									grib.message().section_1_.subcenter());
		__capitalize_write__(grib.message(),info,root_cap_dir_name,fmt_cap,GRIB); //TODO
		result.add_info(info);
	}while(grib.next_message());
	return result;
}