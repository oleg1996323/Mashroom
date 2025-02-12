#pragma once
#include <filesystem>
#include <string>

/*
char* fmt = NULL;
    {
        FILE* fmt_f = fopen("format.bin","rb");
        fseek(fmt_f,0,SEEK_END);
        unsigned end=ftell(fmt_f);
        fseek(fmt_f,0,0);
        if(fmt){
            fprintf(stderr,"Internal error. Abort.\n");
            exit(1);
        }
        if(fmt = (char*)malloc(end)==NULL){
            fprintf(stderr,"Memory allocation error. Abort.\n");
            exit(1);
        }
        fread(fmt,sizeof(char),end,fmt_f);
        fclose(fmt_f);
    }
    DirList root;
    DirList* cwd = &root;
    cwd->current = opendir(root_cap_dir_name);
    //here we begin the cycle
    struct dirent* entry_ptr;
    struct stat status;
    for(;;){
        if(entry_ptr = readdir(cwd->current)==NULL){
            if(cwd->ex){
                closedir(cwd->current);
                cwd = cwd->ex;
                free(cwd->next);
            }
            else return;
        }
        else{
            if(S_ISDIR(status.st_mode)){
                cwd->next = (DirList*)malloc(sizeof(DirList));
                cwd->next->ex = cwd;
                cwd = cwd->next;
                continue;
            }
            else{
                if(S_ISREG(status.st_mode) && S_H){

                }
            }
        }
        
    }
    while(entry_ptr = readdir(cwd->current)!=NULL){

    }

    
    
    S_ISDIR(status.st_mode);
    entry_ptr->d_type == DT_DIR
	while ((entry_ptr = readdir(cwd->current)) != NULL)
        puts (entry_ptr->d_name);
        (void)closedir (dp);
    return 0;
*/

void extract_cpp(){

}