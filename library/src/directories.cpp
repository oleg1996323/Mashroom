#include "filesystem.h"
#include "sys/unistd.h"
#include <sys/stat.h>

bool change_directory(const char* dir){
	if(chdir(dir)!=0){
		if(mkdir(dir,S_IRUSR | S_IWUSR | S_IXUSR)!=0){
			fprintf(stderr,"Error at index. Prompt: cannot create directory");
			return false;
		}
		else{
			if(chdir(dir)!=0){
				fprintf(stderr,"Error at index. Prompt: cannot change directory");
				return false;
			}
			//else printf("Directory changed: %s\n",dir);
		}
	}
	//else printf("Directory changed: %s\n",dir);
	return true;
}

#ifdef __cplusplus
bool change_directory(const fs::path& dir){
	return change_directory(dir.c_str());
}
bool change_directory(const std::string& dir){
	return change_directory(dir.c_str());
}
bool change_directory(std::string_view dir){
	return change_directory(dir.data());
}
#endif