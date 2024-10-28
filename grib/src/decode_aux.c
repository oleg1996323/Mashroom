#include "decode_aux.h"

bool change_directory(const char* dir){
	if(chdir(dir)!=0){
		if(mkdir(dir,S_IRUSR | S_IWUSR | S_IXUSR)!=0){
			fprintf(stderr,"Error at capitalize. Prompt: cannot create directory");
			return false;
		}
		else{
			if(chdir(dir)!=0){
				fprintf(stderr,"Error at capitalize. Prompt: cannot change directory");
				return false;
			}
			//else printf("Directory changed: %s\n",dir);
		}
	}
	//else printf("Directory changed: %s\n",dir);
	return true;
}

bool check(char ch, struct VALID_CAP_FMT* valid){
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