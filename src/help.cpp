#include "help.h"
#include "cmd_translator.h"
#include <iostream>
#include "check.h"

using namespace translate::token;

void help(){
    std::cout<<"Common using:\n"<<\
        "'mode' [option1 arg1 option2 arg2 option3 arg3 ...]\n\n";

    std::cout<<"Accessible modes:\n"<<\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<" - organize the data by defined hierarchy from unique massive archive.\n"<<\
        translate_from_token<ModeArgs>(ModeArgs::CHECK)<<" - check the fullness of data in different date interval; detect the corrupted files of different format.\n"<<\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<" - extract data by rectangle zone or position in date interval from massive data archive with constant complexity.\n"<<\
        translate_from_token<ModeArgs>(ModeArgs::CONFIG)<<" - add, delete, show accessible user-defined named commands.\n"<<\
        translate_from_token<ModeArgs>(ModeArgs::HELP)<< " - show help.\n\n\n"<<std::endl;

        std::cout<<"Accessible options:\n"<<\
        translate_from_token<translate::token::Command>(Command::THREADS)<<"\t("<<\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::CHECK)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<' '<<\
        ") - define the number of threads used for mode-operation execution. (Warning: be care at using with hard disk drive).\n"<<\

        translate_from_token<Command>(Command::IN_PATH)<<"\t("<<\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::CHECK)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<' '<<\
        ") - define the initial path, where is situated the initial data.\n"<<\
        
        translate_from_token<Command>(Command::OUT_PATH)<<"\t("<<\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::CHECK)<<' '<<\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<' '<<\
        ") - define the output path, where will be situated the mode execution result.\n"<<
        "For "<<translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<" the result is the catalogued by directories data respectively to the defined hierarchy."<<\
        "For "<<translate_from_token<ModeArgs>(ModeArgs::CHECK)<<" the result is the file \""<<miss_files_filename<<"\" with list of missed files in defined interval."<<\
        "For "<<translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<" the result is the files of defined format with data extracted from massive data-file."<<\
        " These files must be catalogued respectively to the defined by called before capitalize-mode's hierarchy\"\"."<<\
        "";
        
        " If the interval end value is not defined, it will be defined as now-date. ";
}