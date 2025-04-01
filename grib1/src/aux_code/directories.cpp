#include "aux_code/directories.h"
#include "sys/unistd.h"
#include <sys/stat.h>

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