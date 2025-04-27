#include "help.h"
#include "cmd_translator.h"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <sstream>
#include "integrity.h"

using namespace translate::token;

struct OpposedTextByColumn{
    std::string_view left;
    std::string right;
};

std::string operator+(const std::string& str,std::string_view str_v){
    return str+std::string(str_v);
}

std::string operator+(std::string_view str_v,const std::string& str){
    return std::string(str_v)+str;
}

std::vector<std::string> split_by_width(std::string_view txt, uint width=10){
    std::vector<std::string> result;
    std::istringstream stream{std::string(txt)};
    std::string word;
    std::string line;
    while(stream>>word){
        if (line.size() + word.size() + 1 <= width) {
            if (!line.empty()) line += " ";
            line += word;
        } else {
            result.push_back(line);
            line = word;
        }
    }
    if (!line.empty()) {
        result.push_back(line);
    }
    return result;
}

void SplitByColumns(std::string_view left_t,std::string_view right_t, uint left_w=50,uint right_w=100){
    std::vector<std::string> left_col = std::move(split_by_width(left_t,left_w));
    std::vector<std::string> right_col = std::move(split_by_width(right_t,right_w));
    size_t maxLines = std::max(left_col.size(), right_col.size());
    for(uint i=0;i<maxLines;++i){
        std::string left_str = (i < left_col.size()) ? std::move(left_col.at(i)) : "";
        std::string right_str = (i < right_col.size()) ? std::move(right_col.at(i)) : "";
        std::cout<<std::left<<std::setw(left_w)<<left_str<<" ";
        std::cout<<std::left<<std::setw(right_w)<<right_str<<std::endl;
    }
}

ErrorCode help(){
    std::cout<<"Common using:\n"<<\
        "{mode} [option1 arg1 option2 arg2 option3 arg3 ...]\n\n";

    std::cout<<"Accessible modes:\n";

    SplitByColumns(translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)," - organize the data by defined hierarchy from unique massive archive.");
    SplitByColumns(translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)," - check the integrity of data in different date interval and detect the corrupted files of different format.");
    SplitByColumns(translate_from_token<ModeArgs>(ModeArgs::EXTRACT)," - extract data by rectangle zone or position in date interval from massive data archive with constant complexity.");
    SplitByColumns(translate_from_token<ModeArgs>(ModeArgs::CONFIG)," - add, delete, show accessible user-defined named commands.");
    SplitByColumns(translate_from_token<ModeArgs>(ModeArgs::HELP)," - show help.\n");

    std::cout<<"Accessible options:\n";
    SplitByColumns(translate_from_token<translate::token::Command>(Command::THREADS)+" ( "+\
    translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+' '+\
    translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)+' '+\
    translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",
    " - define the number of threads used for mode-operation execution. (Warning: be care at using with hard disk drive).\n");

    SplitByColumns(translate_from_token<Command>(Command::IN_PATH)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+' '+\
        translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)+' '+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the initial path, where is situated the initial data.\n");
        
    SplitByColumns(translate_from_token<Command>(Command::OUT_PATH)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+' '+\
        translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)+' '+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the output path, where will be situated the mode execution result.\n");

    SplitByColumns(translate_from_token<Command>(Command::EXTRACT_FORMAT)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the output format of data ( ( "+translate_from_token<FileFormat>(FileFormat::BIN)+" | "+\
        translate_from_token<FileFormat>(FileFormat::GRIB)+" | "+\
        translate_from_token<FileFormat>(FileFormat::TXT)+" ) & "+\
        translate_from_token<FileFormat>(FileFormat::ARCHIVED)+" ).\n");

    SplitByColumns(translate_from_token<Command>(Command::CAPITALIZE_FORMAT)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+" )",\
        " - define the input accepted formats by capitalize mode ( "+translate_from_token<FileFormat>(FileFormat::BIN)+" | "+\
        translate_from_token<FileFormat>(FileFormat::GRIB)+" | "+\
        translate_from_token<FileFormat>(FileFormat::TXT)+" ).\n");
    
    SplitByColumns(translate_from_token<Command>(Command::CAPITALIZE_HIERARCHY)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+" )",\
        " - define the output catalogue directories tree.\n");

    SplitByColumns(translate_from_token<Command>(Command::DATE_FROM)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+' '+
        translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)+' '+
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the start point-time of time interval.\n");

    SplitByColumns(translate_from_token<Command>(Command::DATE_TO)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)+' '+
        translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)+' '+
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the end point-time of time interval.\n");
    
    SplitByColumns(translate_from_token<Command>(Command::POSITION)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the coordinate position to extract (in coord-sys WGS-84 for actual Mashroom version).\n");
    
    SplitByColumns(translate_from_token<Command>(Command::LAT_TOP)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the top latitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be lesser or equal to 90.\n");
    SplitByColumns(translate_from_token<Command>(Command::LAT_BOT)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the bottom latitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be bigger or equal to -90.\n");
    SplitByColumns(translate_from_token<Command>(Command::LON_LEFT)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the left longitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be bigger or equal to -180.\n");
    SplitByColumns(translate_from_token<Command>(Command::LON_RIG)+" ( "+\
        translate_from_token<ModeArgs>(ModeArgs::EXTRACT)+" )",\
        " - define the right longitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be lesser than 180.\n");

    std::cout<<"Arguments using:\n\nThe input arguments must be accompanied by options before. The option and its arguments must be separated by space.\n\n";
    SplitByColumns(translate_from_token<>(Command::THREADS)+" {positive integer}",\
        " - define the number of active threads equal to positive number. If the defined number of threads is bigger than the number of physical core on machine then the number active threads will be \
defined by the number of active physical cores on machine. If the defined number of threads if negative or equal 0 then error and programm exit will be done. Else there will be used the defined number of threads.\n");
    SplitByColumns(translate_from_token<>(Command::OUT_PATH)+" dir:{target output directory}",\
            " - define the directory to which will be sent the execution method results. If directory doesn't exists there will be a try to create it. If creation fails an error and programm exit will be done. \
Else if the path exists, but it is not a directory an error and programm exit will be done.\n");
    SplitByColumns(translate_from_token<>(Command::IN_PATH)+" {target output directory}",\
    " - define the directory from which will be read the initial data. If directory doesn't exists an error and programm exit will be done. \
Else if the path exists, but it is not a directory an error and programm exit will be done.\n");
SplitByColumns(translate_from_token<>(Command::EXTRACT_FORMAT)+" {{bin, grib, txt}|zip}",\
" - define the output format files of extract mode. It is allowed to combine either the \"bin\" or \"grib\" or \"txt\" formats with \"zip\". It is not allowed to set empty or combine \"bin\", \"grib\" and \"txt\". \
If option and arguments are not defined, the default output format is \'txt\'. If only \"zip\" is defined then a zipped \"txt\" format will be defined. Argument's token separator is \'|\'");
SplitByColumns(translate_from_token<>(Command::CAPITALIZE_FORMAT)+" {bin, grib, txt}",\
 " - define the output format files of capitalize mode. It is not allowed to set empty or combine \"bin\", \"grib\" and \"txt\".\
If option and arguments are not defined, the default output format is \'bin\'.\n");
SplitByColumns(translate_from_token<>(Command::CAPITALIZE_HIERARCHY)+" {{h,d,m,y,lat,lon,latlon}:...}",\
 " - define the order of directories tree where capitalize mode will catalogue the generated files. It is not allowed to duplicate the order items and also using \'lat\' and \'lon\' items \
with \'latlon\' item. Argument's token separator is \':\'\n");
SplitByColumns(translate_from_token<>(Command::DATE_FROM)+" {{h,d,m,y}{positive integer}:...}",\
 " - define the start point-time of time interval. It is not allowed to duplicate the time unit items. All time unit items must be defined by positive integer. \
If the defined date is unconsistent an error and programm exit will be done. Argument's token separator is \':\'. Argument's token prefixes are \'h\', \'d\', \'m\', \'y\'.\n");
SplitByColumns(translate_from_token<>(Command::DATE_FROM)+" {{h,d,m,y}{positive integer}:...}",\
 " - define the end point-time of time interval. It is not allowed to duplicate the time unit items. All time unit items must be defined by positive integer. \
If the defined date is unconsistent an error and programm exit will be done. If option and arguments are not defined, the default end point-time is now. \
Argument's token separator is \':\'. Argument's token prefixes are \'h\', \'d\', \'m\', \'y\'.\n");
SplitByColumns(translate_from_token<>(Command::POSITION)+" {float number}:{float number}",\
 " - define the coordinate position to extract (in coord-sys WGS-84 for actual Mashroom version). 1-st value - latitude; 2-nd value - longitude. \
Must be defined by float number (in coord-sys WGS-84 for actual Mashroom version). \
If position is unconsistent an error and programm exit will be done. If coordinate(s) for rectangle zone were defined before an error and programm exit will be done. \
Argument's token separator is \':\'.\n");
SplitByColumns(translate_from_token<>(Command::LAT_TOP)+" {float number}",\
 " - define the top latitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be lesser or equal to 90. \
If latitude is unconsistent an error and programm exit will be done. If coordinate position was defined before an error and programm exit will be done.\n");
SplitByColumns(translate_from_token<>(Command::LAT_BOT)+" {float number}",\
 " - define the bottom latitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be bigger or equal to -90. \
If latitude is unconsistent an error and programm exit will be done. If coordinate position was defined before an error and programm exit will be done.\n");
SplitByColumns(translate_from_token<>(Command::LON_LEFT)+" {float number}",\
 " - define the top latitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be bigger or equal to -180. \
If longitude is unconsistent an error and programm exit will be done. If coordinate position was defined before an error and programm exit will be done.\n");
SplitByColumns(translate_from_token<>(Command::LON_RIG)+" {float number}",\
 " - define the right longitude for rectangle zone to extract (in coord-sys WGS-84 for actual Mashroom version). Must be lesser than 180. \
If longitude is unconsistent an error and programm exit will be done. If coordinate position was defined before an error and programm exit will be done.\n");

std::cout<<"Additional info:\n"<<\
"1. If rectangle zone coordinates are correct, but are uncorrectly mixed up, the correction will be done;\n"<<\
"2. If the time-points are correct, but are uncorrectly mixed up, the correction will be done.\n"<<std::endl;
        // "For "<<translate_from_token<ModeArgs>(ModeArgs::CAPITALIZE)<<" the result is the catalogued by directories data respectively to the defined hierarchy."<<\
        // "For "<<translate_from_token<ModeArgs>(ModeArgs::INTEGRITY)<<" the result is the file \""<<miss_files_filename<<"\" with list of missed files in defined interval."<<\
        // "For "<<translate_from_token<ModeArgs>(ModeArgs::EXTRACT)<<" the result is the files of defined format with data extracted from massive data-file."<<\
        // " These files must be catalogued respectively to the defined by called before capitalize-mode's hierarchy\"\"."<<\
        // "";
        
        // " If the interval end value is not defined, it will be defined as now-date. ";
    return ErrorCode::NONE;
}