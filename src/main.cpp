#include "application.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <ranges>
#include "cmd_parse/execute.h"

static int progress_line = 6;

int main(int argc, char* argv[]){
    #ifdef PERMANENT_GET 
        while(int ch =std::getchar()){
            std::cout<<ch<<std::endl;
        }
        //will be added
    #else
    std::cout << "Command-line arguments:" << std::endl;
    if(argc<2){
        std::cout<<"Invalid args. Abort."<<std::endl;
        exit(1);
    }

    std::vector<std::string_view> commands;
    commands.reserve(argc);

    for (int i = 1; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
        commands.push_back(argv[i]);
    }

    execute(commands);
    #endif
    // unsigned int cpus = std::thread::hardware_concurrency();
    // std::filesystem::path path;
    // std::filesystem::path out;
    // MODE mode = MODE::NONE;
    // OrderItems order = OrderItems();
    // Date data_from = Date();
    // Date data_to = Date();
    // Rect rect = Rect();
    // DataExtractMode mode_extract = DataExtractMode::UNDEFINED;
    // cpp::DATA_OUT extract_out_fmt;
    // cpp::DIV_DATA_OUT extract_div_data;
    // Coord coord = Coord();
    // if(strcmp(argv[1],"-ext")==0){
    //     if(MODE::NONE==mode || MODE::EXTRACT==mode)
    //         mode = MODE::EXTRACT;
    //     else{
    //         //std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
    //         exit(1);
    //     }
    // }
    // else if(strcmp(argv[1],"-cap")==0){
    //     if(MODE::NONE==mode || MODE::CAPITALIZE==mode)
    //         mode = MODE::CAPITALIZE;
    //     else{
    //         //std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
    //         exit(1);
    //     }
    // }
    // //enable the checking mode of full full package of downloaded files
    // else if(strcmp(argv[1],"-check")==0){
    //     if(MODE::NONE==mode || MODE::CHECK_ALL_IN_PERIOD==mode)
    //         mode = MODE::CHECK_ALL_IN_PERIOD;
    //     else{
    //         //std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
    //         exit(1);
    //     }
    // }
    // else{
    //     std::cout<<"Missing mode operation 1st argument. Abort."<<std::endl;
    //     exit(1);
    // }
    // for(int i = 2;i<argc;++i){
    //     if(strcmp(argv[i],"-j")==0){
    //         long tmp = std::stol(argv[++i]);
    //         if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
    //             cpus = tmp;
    //     }
    //     else if(strcmp(argv[i],"-p")==0){
    //         path = argv[++i];
    //         if(!std::filesystem::is_directory(path)){
    //             std::cout<<"Error at path argument: not directory. Abort."<<std::endl;
    //             exit(1);
    //         }
    //     }
    //     //date from for extraction
    //     //input separated by ':' with first tokens ('h','d','m','y'),integer values
    //     else if(strcmp(argv[i],"-dfrom")==0){
    //         ++i;
    //         char* str = strtok(argv[i],":");
    //         while(str){
    //             if(strlen(str)>0){
    //                 if(str[0]=='h'){
    //                     data_from.hour = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='y'){
    //                     data_from.year = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='m'){
    //                     data_from.month = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='d'){
    //                     data_from.day = std::stoi(&str[1]);
    //                 }
    //                 else{
    //                     std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
    //                     exit(1);
    //                 }
    //             }
    //             str = strtok(nullptr,":");
    //         }
    //     }
    //     //date to for extraction
    //     else if(strcmp(argv[i],"-dto")==0){
    //         ++i;
    //         char* str = strtok(argv[i],":");
    //         while(str){
    //             if(strlen(str)>0){
    //                 if(str[0]=='h'){
    //                     data_to.hour = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='y'){
    //                     data_to.year = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='m'){
    //                     data_to.month = std::stoi(&str[1]);
    //                 }
    //                 else if(str[0]=='d'){
    //                     data_to.day = std::stoi(&str[1]);
    //                 }
    //                 else{
    //                     std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
    //                     exit(1);
    //                 }
    //             }
    //             str = strtok(nullptr,":");
    //         }
    //     }
    //     // else if(strcmp(argv[i],"-coord")==0){
    //     //     if(mode_extract == DataExtractMode::RECT){
    //     //         std::cout<<"Conflict between arguments. Already choosen extraction mode by zone-rectangle. Abort"<<std::endl;
    //     //         exit(1);
    //     //     }
    //     //     else if(mode_extract == DataExtractMode::POSITION){
    //     //         std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //     //         continue;
    //     //     }
    //     //     else mode_extract=DataExtractMode::POSITION;
    //     //     ++i;
    //     //     char* str = strtok(argv[i],":");
    //     //     coord.lat_ = std::stod(str);
    //     //     str = strtok(argv[i],":");
    //     //     coord.lon_ = std::stod(str);
    //     // }
    //     //input integer or float value with '.' separation
    //     else if(strcmp(argv[i],"-lattop")==0){
    //         if(mode_extract == DataExtractMode::POSITION){
    //             std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
    //             exit(1);
    //         }
    //         else if(mode_extract == DataExtractMode::RECT){
    //             std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //             continue;
    //         }
    //         else mode_extract = DataExtractMode::RECT;
    //         ++i;
    //         rect.y1 = std::stod(argv[i]);
    //     }
    //     //input integer or float value with '.' separation
    //     else if(strcmp(argv[i],"-latbot")==0){
    //         if(mode_extract == DataExtractMode::POSITION){
    //             std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
    //             exit(1);
    //         }
    //         else if(mode_extract == DataExtractMode::RECT){
    //             std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //             continue;
    //         }
    //         else mode_extract = DataExtractMode::RECT;
    //         ++i;
    //         rect.y2 = std::stod(argv[i]);
    //     }
    //     //input integer or float value with '.' separation
    //     else if(strcmp(argv[i],"-lonleft")==0){
    //         if(mode_extract == DataExtractMode::POSITION){
    //             std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
    //             exit(1);
    //         }
    //         else if(mode_extract == DataExtractMode::RECT){
    //             std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //             continue;
    //         }
    //         else mode_extract = DataExtractMode::RECT;
    //         ++i;
    //         rect.x1 = std::stod(argv[i]);
    //     }
    //     //input integer or float value with '.' separation
    //     else if(strcmp(argv[i],"-lonrig")==0){
    //         if(mode_extract == DataExtractMode::POSITION){
    //             std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
    //             exit(1);
    //         }
    //         else if(mode_extract == DataExtractMode::RECT){
    //             std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //             continue;
    //         }
    //         else mode_extract = DataExtractMode::RECT;
    //         ++i;
    //         rect.x2 = std::stod(argv[i]);
    //     }
    //     else if(strcmp(argv[i],"-hier")==0 && mode==MODE::CAPITALIZE ){
    //         if(mode != MODE::CAPITALIZE){
    //             std::cout<<"Argument type -hier mismatch the choosen mode.  Abort."<<std::endl;
    //             exit(1);
    //         }
    //         int order_count = 0;
    //         ++i;
    //         char* str = strtok(argv[i],":");
    //         while(str){
    //             if(strlen(str)>0){
    //                 if(strcmp(str,"h")==0){
    //                     if(order.hour!=-1){
    //                         std::cout<<"Already defined \"hour\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.hour = order_count++;
    //                 }
    //                 else if(strcmp(str,"y")==0){
    //                     if(order.year!=-1){
    //                         std::cout<<"Already defined \"year\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.year = order_count++;
    //                 }
    //                 else if(strcmp(str,"m")==0){
    //                     if(order.month!=-1){
    //                         std::cout<<"Already defined \"month\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.month = order_count++;
    //                 }
    //                 else if(strcmp(str,"d")==0){
    //                     if(order.day!=-1){
    //                         std::cout<<"Already defined \"day\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.day = order_count++;
    //                 }
    //                 else if(strcmp(str,"lat")==0){
    //                     if(order.lat!=-1){
    //                         std::cout<<"Already defined \"lat\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.lat = order_count++;
    //                 }
    //                 else if(strcmp(str,"lon")==0){
    //                     if(order.lon!=-1){
    //                         std::cout<<"Already defined \"lon\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else order.lon = order_count++;
    //                 }
    //                 else if(strcmp(str,"latlon")==0){
    //                     if(order.lat!=-1 || order.lon!=-1){
    //                         std::cout<<"Already defined \"lat or lon\" hierarchy capitalize mode item. Abort."<<std::endl;
    //                     }
    //                     else{
    //                         order.lat = order_count;
    //                         order.lon = order_count++;
    //                     }
    //                 }
    //                 else{
    //                     std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
    //                     exit(1);
    //                 }
    //             }
    //             str = strtok(nullptr,":");
    //         }
    //     }
    //     else if(strcmp(argv[i],"-format")==0){
    //         ++i;
    //         if(strcmp(argv[i],"bin")==0){
    //             if(order.fmt==NONE)
    //                 order.fmt = BINARY;
    //             else{
    //                 std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
    //                 exit(1);
    //             }
    //         }
    //         else if(strcmp(argv[i],"txt")==0){
    //             if(order.fmt==NONE)
    //                 order.fmt = TEXT;
    //             else{
    //                 std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
    //                 exit(1);
    //             }
    //         }
    //         else if(strcmp(argv[i],"grib")==0){
    //             if(order.fmt==NONE)
    //                 order.fmt = GRIB;
    //             else{
    //                 std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
    //                 exit(1);
    //             }
    //         }
    //     }
    //     else if(strcmp(argv[i],"-send")==0){
    //         ++i;
    //         char* arg = argv[i];
    //         char* tokens;
    //         char* type = strtok_r(arg,":",&tokens);
    //         if(strcmp(type,"dir")==0){
    //             out = strtok_r(NULL,":",&tokens);
    //             if(!std::filesystem::is_directory(out)){
    //                 if(!std::filesystem::create_directory(out)){
    //                     std::cout<<out<<" is not a directory. Abort.";
    //                     exit(1);
    //                 }
    //             }
    //         }
    //         else if(strcmp(type,"ip")==0 && mode==MODE::EXTRACT){
    //             out = strtok_r(NULL,":",&tokens);
    //         }
    //         else{
    //             std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
    //             exit(1);
    //         }
    //     }
    //     else if(strcmp(argv[i],"-extfmt")==0){
    //         ++i;
    //         if(extract_out_fmt==-1 || extract_out_fmt==cpp::DATA_OUT::ARCHIVED)
    //             if(strcmp(argv[i],"zip")==0){
    //                 extract_out_fmt=(cpp::DATA_OUT)(extract_out_fmt&cpp::DATA_OUT::ARCHIVED);
    //                 ++i;
    //             }
    //         if(strcmp(argv[i],"txt")==0 && (extract_out_fmt==cpp::DATA_OUT::DEFAULT || extract_out_fmt==cpp::DATA_OUT::ARCHIVED)){
    //             extract_out_fmt=(cpp::DATA_OUT)(extract_out_fmt|cpp::DATA_OUT::TXT_F);
    //             ++i;
    //         }
    //         else if(strcmp(argv[i],"bin")==0){
    //             extract_out_fmt=(cpp::DATA_OUT)(extract_out_fmt|cpp::DATA_OUT::BIN_F);
    //             ++i;
    //         }
    //         else if(strcmp(argv[i],"grib")==0){
    //             extract_out_fmt=(cpp::DATA_OUT)(extract_out_fmt|cpp::DATA_OUT::GRIB_F);
    //             ++i;
    //         }
    //         else{
    //             std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
    //             exit(1);
    //         }
    //         if(strcmp(argv[i],"zip")==0){
    //             if((!(extract_out_fmt&cpp::DATA_OUT::DEFAULT) && !(extract_out_fmt&cpp::DATA_OUT::ARCHIVED))){
    //                 extract_out_fmt=(cpp::DATA_OUT)(extract_out_fmt|cpp::DATA_OUT::ARCHIVED);
    //                 ++i;
    //             }
    //             else{
    //                 std::cout<<"Undefined output type format: argv["<<argv[i]<<"]"<<std::endl;
    //                 exit(1);
    //             }
    //         }
    //         continue;
    //     }
    //     else if(strcmp(argv[i],"-divby")==0){
    //         ++i;
    //         if(strcmp(argv[i],"h")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::HOUR_T;
    //         else if(strcmp(argv[i],"y")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::YEAR_T;
    //         else if(strcmp(argv[i],"m")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::MONTH_T;
    //         else if(strcmp(argv[i],"d")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::DAY_T;
    //         else if(strcmp(argv[i],"lat")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::LAT;
    //         else if(strcmp(argv[i],"lon")==0)
    //             extract_div_data = cpp::DIV_DATA_OUT::LON;
    //         else if(strcmp(argv[i],"latlon")==0)
    //             extract_div_data = (cpp::DIV_DATA_OUT)(cpp::DIV_DATA_OUT::LAT|cpp::DIV_DATA_OUT::LON);
    //         else{
    //             std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
    //             exit(1);
    //         }
    //     }
    //     else if(strcmp(argv[i],"-pos")==0){
    //         ++i;
    //         char* arg = argv[i];
    //         char* tokens;
    //         size_t pos = 0;
    //         if(mode_extract == DataExtractMode::RECT){
    //             std::cout<<"Conflict between arguments. Already choosen extraction mode by zone-rectangle. Abort"<<std::endl;
    //             exit(1);
    //         }
    //         else if(mode_extract == DataExtractMode::POSITION){
    //             std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
    //             continue;
    //         }
    //         else mode_extract=DataExtractMode::POSITION;
    //         try{
    //             coord.lat_ = std::stof(std::string(strtok_r(arg,":",&tokens)),&pos);
    //             if(pos!=std::strlen(arg))
    //                 throw std::invalid_argument("Invalid lat value argument");
    //             coord.lon_ = std::stof(std::string(strtok_r(arg,":",&tokens)));
    //             if(pos!=std::strlen(arg))
    //                 throw std::invalid_argument("Invalid lon value argument");
    //         }
    //         catch(const std::invalid_argument& err){std::cout<<err.what()<<std::endl; exit(1);}
    //         catch(const std::out_of_range){std::cout<<"Error at position definition"<<std::endl;exit(1);}
    //     }
    //     else{
    //         std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
    //         exit(1);
    //     }
    // }
    // std::ofstream fmt(path/"format.bin",std::ios::trunc|std::ios::binary);
    // if(!fmt.is_open()){
    //     std::cout<<"Cannot open "<<path/"format.bin"<<std::endl;
    //     exit(1);
    // }
    // else{
    //     std::cout<<"Openned "<<path/"format.bin"<<std::endl;
    // }
    // fmt.write("YM",2);
    // fmt.close();
    // {
    //     // Отключаем канонический режим и эхо (для чтения ответа терминала)
    //     termios oldt;
    //     tcgetattr(STDIN_FILENO, &oldt);
    //     termios newt = oldt;
    //     newt.c_lflag &= ~(ICANON | ECHO);
    //     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    //     int row,col;
    //     // if(getCursorPosition(row,col))
    //     //     progress_line = row;
    //     // else exit(1);
    // }
    // if(mode==MODE::CHECK_ALL_IN_PERIOD){}
    //     // if(missing_files(path,cpus,1d/1/1991))
    //     //     std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
    //     // else std::cout<<"No missing files detected"<<std::endl;
    // else if(mode==MODE::CAPITALIZE)
    //     capitalize_cpp(path,out,order);
    // else if(mode==MODE::EXTRACT){
    //     if(mode_extract==DataExtractMode::POSITION)
    //         cpp::extract_cpp_pos(path,out,data_from,data_to,coord,extract_out_fmt);
    //     else if(mode_extract==DataExtractMode::RECT)
    //         cpp::extract_cpp_rect(path,data_from,data_to,rect,extract_out_fmt);
    //     else {
    //         std::cout<<"Undefined extraction data mode. Abort."<<std::endl;
    //     }
    // }
    // else
    //     std::cout<<"Error parsing args. Abort."<<std::endl;
    // // std::filesystem::path path = std::filesystem::current_path();
    // // ExtractData extract_ = ExtractData();
    // // // extract_.bound.x1=20;
    // // // extract_.bound.x2=21;
    // // // extract_.bound.y1=50;
    // // // extract_.bound.y2=51;
    // // extract_.bound = Rect();
    // // extract_.date.year=2017;
    // // extract_.date.month=5;
    // // extract_.date.day=15;
    // // extract_.date.hour=12;
    // // extract_.data_name = "2T";
    // // ValueByCoord* values;
    // // long count=1;
    // // unsigned long pos = 0;

    // // ColorAtValue<RGB<16>> violet_220 = {{102,0,214},200};
    // // ColorAtValue<RGB<16>> blue_240 = {{0,128,UCHAR_MAX},220};
    // // ColorAtValue<RGB<16>> whiteblue_250 = {{204,UCHAR_MAX,UCHAR_MAX},240};
    // // ColorAtValue<RGB<16>> lightgreen_260 = {{102,UCHAR_MAX,102},260};
    // // ColorAtValue<RGB<16>> green_270 = {{0,UCHAR_MAX,0},280};
    // // ColorAtValue<RGB<16>> yellow_280 = {{UCHAR_MAX,UCHAR_MAX,0},300};
    // // ColorAtValue<RGB<16>> red_290 = {{UCHAR_MAX,0,0},320};
    // // //ColorAtValue blue_240 = {{0,128,UCHAR_MAX},UCHAR_MAX};
    // // //ColorAtValue red_290 = {{UCHAR_MAX,0,0},320};
    // // violet_220.color=violet_220.color*((uint8_t)255);
    // // std::vector<ColorAtValue<RGB<16>>> grad = {violet_220,blue_240,whiteblue_250,lightgreen_260,green_270,yellow_280,red_290};
    // // //std::vector<ColorAtValue> grad = {blue_240,red_290};
    // // for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path)){
    // //     if(entry.is_regular_file())
    // //         if(entry.path().has_extension() && (entry.path().extension()==".grib" || entry.path().extension()==".grb")){

    // //             //std::cout<<entry.path()<<std::endl;
    // //             GridData grid = extract(&extract_, entry.path().c_str(),&values,&count,&pos);
    // //             Array_2D arr;
    // //             count = 1;
    // //             pos = 0;
    // //             auto res = extract_ptr(&extract_,entry.path().c_str(),&count,&pos);
    // //             arr.data = res.values;
    // //             arr.nx = res.nx;
    // //             arr.ny = res.ny;
    // //             arr.dx = grid.dx;
    // //             arr.dy = grid.dy;
    // //             Array_2D result = bilinear_interpolation(arr,res.nx*4,res.ny*4);
    // //             PngOutGray("temp_1990011512_X4xX4_gray.png",result.data,Size(result.nx,result.ny));
    // //             PngOutRGBGradient<8>("temp_1990011512_X4xX4.png",result.data,Size(result.nx,result.ny),grad,true);
    // //             //PngOut("temp_1990011512_X4xX4.png",result.data,result.nx,result.ny,grad);
    // //         }
        
    // // }
    return 0;
}