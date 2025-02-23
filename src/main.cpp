#ifdef __cplusplus
extern "C"{
    #include <decode.h>
    #include <capitalize.h>
    #include <extract.h>
    #include <interpolation.h>
    #include <structures.h>
    #include <contains.h>
}
#endif
#include <dlfcn.h>
#include <termios.h>
#include <memory>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include "color.h"
#include "png_out.h"
#include <chrono>
#include <algorithm>
#include <ranges>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <cassert>
#include <print>
#include <thread>
#include <future>
#include <cstdio>
#include "log_duration.h"
#include "cap.h"
#include "extract_cpp.h"
#include <experimental/executor>
#include <experimental/buffer>
#include <experimental/internet>
#include <experimental/io_context>
#include <experimental/socket>
#include <experimental/timer>
#include <filesystem>
#include <fstream>

static int progress_line = 6;

// Функция для получения текущей позиции курсора
bool getCursorPosition(int& row, int& col) {
    // Отправляем запрос позиции курсора
    std::cout << "\033[6n" << std::flush<<'\r'<<std::flush;

    // Читаем ответ терминала
    char buf[32];
    int i = 0;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break; // Конец ответа
        i++;
    }
    buf[i] = '\0';

    // Парсим ответ (формат: \033[<row>;<col>R)
    if (buf[0] != '\033' || buf[1] != '[') return false;
    if (sscanf(&buf[2], "%d;%d", &row, &col) != 2) return false;

    return true;
}

class pBar {
public:
    pBar(){}

    void setline(int pline){
        lineNumber = pline;
    }

    void update(double newProgress) {
        currentProgress = newProgress;
        amountOfFiller = (int)((currentProgress / neededProgress) * (double)pBarLength);
    }

    void print() {
        currUpdateVal %= pBarUpdater.length();
        std::cout << "\033[" << lineNumber<< ";0H"; // Переместить курсор на нужную строку
        std::cout << firstPartOfpBar; // Начало прогресс-бара
        for (int a = 0; a < amountOfFiller; a++) { // Заполнение прогресса
            std::cout << pBarFiller;
        }
        std::cout << pBarUpdater[currUpdateVal];
        for (int b = 0; b < pBarLength - amountOfFiller; b++) { // Пробелы
            std::cout << " ";
        }
        std::cout << lastPartOfpBar // Конец прогресс-бара
                  << " Progress: " << (int)(100 * (currentProgress / neededProgress)) << "% " // Процент
                  << std::flush;
        currUpdateVal += 1;
    }

    std::string firstPartOfpBar = "[", // Начало прогресс-бара
                lastPartOfpBar = "]",  // Конец прогресс-бара
                pBarFiller = "|",      // Символ заполнения
                pBarUpdater = "/-\\|"; // Анимация

private:
    int amountOfFiller,
        pBarLength = 50, // Длина прогресс-бара
        currUpdateVal = 0, // Текущее значение анимации
        lineNumber = 0; // Номер строки для вывода
    double currentProgress = 0, // Текущий прогресс
           neededProgress = 100; // Общий прогресс
};

template<class Clock, class Duration> 
struct std::incrementable_traits<std::chrono::time_point<Clock, Duration>> { 
  using difference_type = typename Duration::rep;
};

struct ErrorFiles{
    std::filesystem::path name;
    ErrorCodes code;
};

struct ProcessResult{
    std::set<std::chrono::sys_days> found;
    std::vector<ErrorFiles> err_files;
};

ProcessResult process_core(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print = nullptr) {
    ProcessResult result;
    for (const std::filesystem::directory_entry& entry : entries) {
        if (entry.is_regular_file() && entry.path().has_extension() && 
            (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::ifstream f(entry.path(), std::ios::in);
            if (!f.is_open()) {
                std::cerr << "Cannot open " << entry.path() << std::endl;
                continue;
            }
            f.seekg(0, std::ios::end);
            long int max_pos = f.tellg();
            f.seekg(0, std::ios::beg);
            f.close();

            unsigned long int pos = 0;
            long int count = 0;
            int p_line;
            pBar bar;

            {
                if (mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    p_line = progress_line++;
                    bar.setline(p_line);
                    std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    bar.update(0);
                    bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
                else{
                    p_line = progress_line++;
                    bar.setline(p_line);
                    std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    bar.update(0);
                    bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
            }

            while (pos < max_pos) {

                auto data = get_from_pos(entry.path().c_str(), &count, &pos);
                if(data.code==0)
                    result.found.insert(std::chrono::sys_days(std::chrono::year_month_day(
                        std::chrono::year(data.data.date.year),
                        std::chrono::month(data.data.date.month),
                        std::chrono::day(data.data.date.day))));
                else{
                    if (mute_at_print){
                        std::lock_guard<std::mutex> locked(*mute_at_print);
                        std::cout << "Error occured. Code "<<data.code<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                        result.err_files.emplace_back(ErrorFiles{entry.path(),data.code});
                        break;
                    }
                    else {
                        std::cout << "Error occured. Code "<<data.code<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                        result.err_files.emplace_back(ErrorFiles{entry.path(),data.code});
                        break;
                    }
                }

                {
                    if (mute_at_print){
                        std::lock_guard<std::mutex> locked(*mute_at_print);
                        bar.update((double)pos/(double)max_pos*100);
                        bar.print();
                        std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                    }
                    else {
                        bar.update((double)pos/(double)max_pos*100);
                        bar.print();
                        std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                    }
                }
            }
        }
    }
    return result;
}


bool missing_files(std::filesystem::path path,unsigned int cpus, std::chrono::year_month_day from, std::chrono::year_month_day to = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())){
    bool result = false;
    std::set<std::chrono::sys_days> found;
    auto begin_days = std::chrono::sys_days(from);
    auto end_days = std::chrono::sys_days(to)+std::chrono::days(1);
    auto days_range = std::views::iota(begin_days,end_days);
    std::ofstream missing_log(std::filesystem::current_path()/"missing_files.txt",std::ios::out);
    if(!missing_log.is_open()){
        std::cerr<<"Cannot open file: \"missing_files.txt\""<<std::endl;
    }
    auto initial = from;
    std::vector<std::filesystem::directory_entry> entries;
    for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path))
        entries.push_back(entry);
    {
        if(entries.size()/cpus>1){
            std::vector<std::thread> threads(cpus);
            std::vector<std::promise<ProcessResult>> threads_results(cpus);

            for(unsigned int cpu = 0;cpu<cpus && entries.size()/cpus>1;++cpu){
                auto r = std::ranges::subrange(entries.begin()+cpu*entries.size()/cpus,
                                                                                    entries.begin()+(cpu+1)*entries.size()/cpus<entries.end()?
                                                                                    entries.begin()+(cpu+1)*entries.size()/cpus:
                                                                                    entries.end()
                                                                                    );
                std::mutex mute_at_print;
                std::promise<ProcessResult>* prom = &threads_results.at(cpu);
                threads.at(cpu) = std::move(std::thread([r,prom,&mute_at_print]() mutable{
                                    prom->set_value(process_core(std::move(r),&mute_at_print));
                                }));
            }
            for(int i = 0;i<cpus;++i){
                ProcessResult res = std::move(threads_results.at(i).get_future().get());
                if(!res.err_files.empty()){
                    std::ofstream err_log("error_files_log.txt",std::ios::trunc);
                    if(err_log.is_open())
                    for(ErrorFiles& err_f:res.err_files){
                        err_log<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                    }
                }
                found.merge(res.found);
                threads.at(i).join();
            }
            threads.clear();
            threads_results.clear();
        }
        else{
            ProcessResult res = process_core(entries);
            if(!res.err_files.empty()){
                std::ofstream err_log("error_files_log.txt",std::ios::trunc);
                if(err_log.is_open())
                for(ErrorFiles& err_f:res.err_files){
                    err_log<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                }
            }
            found = res.found;
        }
    }
    
    std::vector<std::chrono::sys_days> miss_data;
    std::ranges::set_difference(days_range,found,std::back_inserter(miss_data), std::ranges::less());
    if(!miss_data.empty()){
        std::set<std::chrono::year_month> by_months;
        std::transform(miss_data.begin(),miss_data.end(),std::inserter(by_months,by_months.begin()),[](const std::chrono::sys_days& date){
            std::chrono::year_month_day tmp(date);
            return std::chrono::year_month(tmp.year(),tmp.month());
            });
        result = true;
        for(auto date:by_months)
            missing_log<<"("<<date.year()<<","<<date.month()<<")"<<std::endl;
    }
    missing_log.close();
    return result;
}

using namespace std::chrono_literals;

enum class MODE{
    NONE,
    CHECK_ALL_IN_PERIOD,
    CAPITALIZE,
    EXTRACT
};

std::string get_string_mode(MODE mode){
    switch(mode){
        case(MODE::NONE):
            return "None mode";
        case(MODE::CAPITALIZE):
            return "Capitalize mode (arg = -cap)";
        case(MODE::CHECK_ALL_IN_PERIOD):
            return "Check mode (arg = -check)";
        case(MODE::EXTRACT):
            return "Extract mode (arg = -ext)";
        default:{
            std::cout<<"Argument error at \"get_string_mode\""<<std::endl;
            exit(1);
        }
    }
}

#include <variant>

enum DATA_OUT{
    DEFAULT= 0,
    TXT_F = 1,
    BIN_F = 2,
    GRIB_F = 3<<1,
    ARCHIVED = 4<<2,
};

//separation by files
enum DIV_DATA_OUT{
    ALL_IN_ONE = 0, //all in one file with data inline
    YEAR_T = 1<<0,
    MONTH_T = 1<<2,
    DAY_T = 1<<3,
    HOUR_T = 1<<4,
    LAT = 1<<5,
    LON = 1<<6
};

enum class DataExtractMode{
    UNDEFINED,
    POSITION,
    RECT
};

int main(int argc, char* argv[]){
    std::cout << "Command-line arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    unsigned int cpus = std::thread::hardware_concurrency();
    std::filesystem::path path;
    std::filesystem::path out;
    MODE mode = MODE::NONE;
    OrderItems order = OrderItems();
    Date data_from = Date();
    Date data_to = Date();
    Rect rect = Rect();
    DataExtractMode mode_extract = DataExtractMode::UNDEFINED;
    DATA_OUT extract_out_fmt;
    DIV_DATA_OUT extract_div_data;
    Coord coord = Coord();
    if(strcmp(argv[1],"-ext")==0){
        if(MODE::NONE==mode || MODE::EXTRACT==mode)
            mode = MODE::EXTRACT;
        else{
            std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
            exit(1);
        }
    }
    else if(strcmp(argv[1],"-cap")==0){
        if(MODE::NONE==mode || MODE::CAPITALIZE==mode)
            mode = MODE::CAPITALIZE;
        else{
            std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
            exit(1);
        }
    }
    //enable the checking mode of full full package of downloaded files
    else if(strcmp(argv[1],"-check")==0){
        if(MODE::NONE==mode || MODE::CHECK_ALL_IN_PERIOD==mode)
            mode = MODE::CHECK_ALL_IN_PERIOD;
        else{
            std::cout<<"Incorrect argument. Already choosen mode: "<<get_string_mode(mode)<<std::endl;
            exit(1);
        }
    }
    else{
        std::cout<<"Missing mode operation 1st argument. Abort."<<std::endl;
        exit(1);
    }
    for(int i = 2;i<argc;++i){
        if(strcmp(argv[i],"-j")==0){
            long tmp = std::stol(argv[++i]);
            if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
                cpus = tmp;
        }
        else if(strcmp(argv[i],"-p")==0){
            path = argv[++i];
            if(!std::filesystem::is_directory(path)){
                std::cout<<"Error at path argument: not directory. Abort."<<std::endl;
                exit(1);
            }
        }
        //date from for extraction
        //input separated by ':' with first tokens ('h','d','m','y'),integer values
        else if(strcmp(argv[i],"-dfrom")==0){
            ++i;
            char* str = strtok(argv[i],":");
            while(str){
                if(strlen(str)>0){
                    if(str[0]=='h'){
                        data_from.hour = std::stoi(&str[1]);
                    }
                    else if(str[0]=='y'){
                        data_from.year = std::stoi(&str[1]);
                    }
                    else if(str[0]=='m'){
                        data_from.month = std::stoi(&str[1]);
                    }
                    else if(str[0]=='d'){
                        data_from.day = std::stoi(&str[1]);
                    }
                    else{
                        std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                        exit(1);
                    }
                }
                str = strtok(nullptr,":");
            }
        }
        //date to for extraction
        else if(strcmp(argv[i],"-dto")==0){
            ++i;
            char* str = strtok(argv[i],":");
            while(str){
                if(strlen(str)>0){
                    if(str[0]=='h'){
                        data_to.hour = std::stoi(&str[1]);
                    }
                    else if(str[0]=='y'){
                        data_to.year = std::stoi(&str[1]);
                    }
                    else if(str[0]=='m'){
                        data_to.month = std::stoi(&str[1]);
                    }
                    else if(str[0]=='d'){
                        data_to.day = std::stoi(&str[1]);
                    }
                    else{
                        std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                        exit(1);
                    }
                }
                str = strtok(nullptr,":");
            }
        }
        // else if(strcmp(argv[i],"-coord")==0){
        //     if(mode_extract == DataExtractMode::RECT){
        //         std::cout<<"Conflict between arguments. Already choosen extraction mode by zone-rectangle. Abort"<<std::endl;
        //         exit(1);
        //     }
        //     else if(mode_extract == DataExtractMode::POSITION){
        //         std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
        //         continue;
        //     }
        //     else mode_extract=DataExtractMode::POSITION;
        //     ++i;
        //     char* str = strtok(argv[i],":");
        //     coord.lat_ = std::stod(str);
        //     str = strtok(argv[i],":");
        //     coord.lon_ = std::stod(str);
        // }
        //input integer or float value with '.' separation
        else if(strcmp(argv[i],"-lattop")==0){
            if(mode_extract == DataExtractMode::POSITION){
                std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
                exit(1);
            }
            else if(mode_extract == DataExtractMode::RECT){
                std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
                continue;
            }
            else mode_extract = DataExtractMode::RECT;
            ++i;
            rect.y1 = std::stod(argv[i]);
        }
        //input integer or float value with '.' separation
        else if(strcmp(argv[i],"-latbot")==0){
            if(mode_extract == DataExtractMode::POSITION){
                std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
                exit(1);
            }
            else if(mode_extract == DataExtractMode::RECT){
                std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
                continue;
            }
            else mode_extract = DataExtractMode::RECT;
            ++i;
            rect.y2 = std::stod(argv[i]);
        }
        //input integer or float value with '.' separation
        else if(strcmp(argv[i],"-lonleft")==0){
            if(mode_extract == DataExtractMode::POSITION){
                std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
                exit(1);
            }
            else if(mode_extract == DataExtractMode::RECT){
                std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
                continue;
            }
            else mode_extract = DataExtractMode::RECT;
            ++i;
            rect.x1 = std::stod(argv[i]);
        }
        //input integer or float value with '.' separation
        else if(strcmp(argv[i],"-lonrig")==0){
            if(mode_extract == DataExtractMode::POSITION){
                std::cout<<"Conflict between arguments. Already choosen extraction mode by coordinate position. Abort"<<std::endl;
                exit(1);
            }
            else if(mode_extract == DataExtractMode::RECT){
                std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
                continue;
            }
            else mode_extract = DataExtractMode::RECT;
            ++i;
            rect.x2 = std::stod(argv[i]);
        }
        else if(strcmp(argv[i],"-hier")==0 && mode==MODE::CAPITALIZE ){
            if(mode != MODE::CAPITALIZE){
                std::cout<<"Argument type -hier mismatch the choosen mode.  Abort."<<std::endl;
                exit(1);
            }
            int order_count = 0;
            ++i;
            char* str = strtok(argv[i],":");
            while(str){
                if(strlen(str)>0){
                    if(strcmp(str,"h")==0){
                        if(order.hour!=-1){
                            std::cout<<"Already defined \"hour\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.hour = order_count++;
                    }
                    else if(strcmp(str,"y")==0){
                        if(order.year!=-1){
                            std::cout<<"Already defined \"year\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.year = order_count++;
                    }
                    else if(strcmp(str,"m")==0){
                        if(order.month!=-1){
                            std::cout<<"Already defined \"month\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.month = order_count++;
                    }
                    else if(strcmp(str,"d")==0){
                        if(order.day!=-1){
                            std::cout<<"Already defined \"day\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.day = order_count++;
                    }
                    else if(strcmp(str,"lat")==0){
                        if(order.lat!=-1){
                            std::cout<<"Already defined \"lat\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.lat = order_count++;
                    }
                    else if(strcmp(str,"lon")==0){
                        if(order.lon!=-1){
                            std::cout<<"Already defined \"lon\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else order.lon = order_count++;
                    }
                    else if(strcmp(str,"latlon")==0){
                        if(order.lat!=-1 || order.lon!=-1){
                            std::cout<<"Already defined \"lat or lon\" hierarchy capitalize mode item. Abort."<<std::endl;
                        }
                        else{
                            order.lat = order_count;
                            order.lon = order_count++;
                        }
                    }
                    else{
                        std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                        exit(1);
                    }
                }
                str = strtok(nullptr,":");
            }
        }
        else if(strcmp(argv[i],"-format")==0){
            ++i;
            if(strcmp(argv[i],"bin")==0){
                if(order.fmt==NONE)
                    order.fmt = BINARY;
                else{
                    std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[i],"txt")==0){
                if(order.fmt==NONE)
                    order.fmt = TEXT;
                else{
                    std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[i],"grib")==0){
                if(order.fmt==NONE)
                    order.fmt = GRIB;
                else{
                    std::cout<<"Already defined file format output at capitalize mode. Abort."<<std::endl;
                    exit(1);
                }
            }
        }
        else if(strcmp(argv[i],"-send")==0){
            ++i;
            char* arg = argv[i];
            char* tokens;
            char* type = strtok_r(arg,":",&tokens);
            if(strcmp(type,"dir")==0){
                out = strtok_r(NULL,":",&tokens);
                if(!std::filesystem::is_directory(out)){
                    if(!std::filesystem::create_directory(out)){
                        std::cout<<out<<" is not a directory. Abort.";
                        exit(1);
                    }
                }
            }
            else if(strcmp(type,"ip")==0 && mode==MODE::EXTRACT){
                out = strtok_r(NULL,":",&tokens);
            }
            else{
                std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
                exit(1);
            }
        }
        else if(strcmp(argv[i],"-extfmt")==0){
            ++i;
            if(extract_out_fmt==-1 || extract_out_fmt==ARCHIVED)
                if(strcmp(argv[i],"zip")==0){
                    extract_out_fmt=(DATA_OUT)(extract_out_fmt&DATA_OUT::ARCHIVED);
                    ++i;
                }
            if(strcmp(argv[i],"txt")==0 && (extract_out_fmt==DEFAULT || extract_out_fmt==ARCHIVED))
                extract_out_fmt=(DATA_OUT)(extract_out_fmt&DATA_OUT::TXT_F);
            else if(strcmp(argv[i],"bin")==0)
                extract_out_fmt=(DATA_OUT)(extract_out_fmt&DATA_OUT::BIN_F);
            else if(strcmp(argv[i],"grib")==0)
                extract_out_fmt=(DATA_OUT)(extract_out_fmt&DATA_OUT::GRIB_F);
            else{
                std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
                exit(1);
            }
            if(strcmp(argv[i],"zip")==0){
                if((!(extract_out_fmt&DEFAULT) && !(extract_out_fmt&ARCHIVED))){
                    extract_out_fmt=(DATA_OUT)(extract_out_fmt&DATA_OUT::ARCHIVED);
                    ++i;
                }
                else{
                    std::cout<<"Undefined output type format: argv["<<argv[i]<<"]"<<std::endl;
                    exit(1);
                }
            }
            continue;
        }
        else if(strcmp(argv[i],"-divby")==0){
            ++i;
            if(strcmp(argv[i],"h")==0)
                extract_div_data = DIV_DATA_OUT::HOUR_T;
            else if(strcmp(argv[i],"y")==0)
                extract_div_data = DIV_DATA_OUT::YEAR_T;
            else if(strcmp(argv[i],"m")==0)
                extract_div_data = DIV_DATA_OUT::MONTH_T;
            else if(strcmp(argv[i],"d")==0)
                extract_div_data = DIV_DATA_OUT::DAY_T;
            else if(strcmp(argv[i],"lat")==0)
                extract_div_data = DIV_DATA_OUT::LAT;
            else if(strcmp(argv[i],"lon")==0)
                extract_div_data = DIV_DATA_OUT::LON;
            else if(strcmp(argv[i],"latlon")==0)
                extract_div_data = (DIV_DATA_OUT)(DIV_DATA_OUT::LAT|DIV_DATA_OUT::LON);
            else{
                std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                exit(1);
            }
        }
        else if(strcmp(argv[i],"-pos")==0){
            ++i;
            char* arg = argv[i];
            char* tokens;
            size_t pos = 0;
            if(mode_extract == DataExtractMode::RECT){
                std::cout<<"Conflict between arguments. Already choosen extraction mode by zone-rectangle. Abort"<<std::endl;
                exit(1);
            }
            else if(mode_extract == DataExtractMode::POSITION){
                std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
                continue;
            }
            else mode_extract=DataExtractMode::POSITION;
            try{
                coord.lat_ = std::stof(std::string(strtok_r(arg,":",&tokens)),&pos);
                if(pos!=std::strlen(arg))
                    throw std::invalid_argument("Invalid lat value argument");
                coord.lon_ = std::stof(std::string(strtok_r(arg,":",&tokens)));
                if(pos!=std::strlen(arg))
                    throw std::invalid_argument("Invalid lon value argument");
            }
            catch(const std::invalid_argument& err){std::cout<<err.what()<<std::endl; exit(1);}
            catch(const std::out_of_range){std::cout<<"Error at position definition"<<std::endl;exit(1);}
        }
        else{
            std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
            exit(1);
        }
    }
    std::ofstream fmt(path/"format.bin",std::ios::trunc|std::ios::binary);
    if(!fmt.is_open()){
        std::cout<<"Cannot open "<<path/"format.bin"<<std::endl;
        exit(1);
    }
    else{
        std::cout<<"Openned "<<path/"format.bin"<<std::endl;
    }
    fmt.write("YM",2);
    fmt.close();
    {
        // Отключаем канонический режим и эхо (для чтения ответа терминала)
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        int row,col;
        if(getCursorPosition(row,col))
            progress_line = row;
        else exit(1);
    }
    if(mode==MODE::CHECK_ALL_IN_PERIOD)
        if(missing_files(path,cpus,1d/1/1991))
            std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
        else std::cout<<"No missing files detected"<<std::endl;
    else if(mode==MODE::CAPITALIZE)
        cap(path,out,order);
    else if(mode==MODE::EXTRACT){
        if(mode_extract==DataExtractMode::POSITION)
            cpp::extract_cpp_pos(path,out,data_from,data_to,coord,GRIB);
        else if(mode_extract==DataExtractMode::RECT)
            cpp::extract_cpp_rect<true,false>(path,data_from,data_to,rect,GRIB);
        else {
            std::cout<<"Undefined extraction data mode. Abort."<<std::endl;
        }
    }
    else
        std::cout<<"Error parsing args. Abort."<<std::endl;
    // std::filesystem::path path = std::filesystem::current_path();
    // ExtractData extract_ = ExtractData();
    // // extract_.bound.x1=20;
    // // extract_.bound.x2=21;
    // // extract_.bound.y1=50;
    // // extract_.bound.y2=51;
    // extract_.bound = Rect();
    // extract_.date.year=2017;
    // extract_.date.month=5;
    // extract_.date.day=15;
    // extract_.date.hour=12;
    // extract_.data_name = "2T";
    // ValueByCoord* values;
    // long count=1;
    // unsigned long pos = 0;

    // ColorAtValue<RGB<16>> violet_220 = {{102,0,214},200};
    // ColorAtValue<RGB<16>> blue_240 = {{0,128,UCHAR_MAX},220};
    // ColorAtValue<RGB<16>> whiteblue_250 = {{204,UCHAR_MAX,UCHAR_MAX},240};
    // ColorAtValue<RGB<16>> lightgreen_260 = {{102,UCHAR_MAX,102},260};
    // ColorAtValue<RGB<16>> green_270 = {{0,UCHAR_MAX,0},280};
    // ColorAtValue<RGB<16>> yellow_280 = {{UCHAR_MAX,UCHAR_MAX,0},300};
    // ColorAtValue<RGB<16>> red_290 = {{UCHAR_MAX,0,0},320};
    // //ColorAtValue blue_240 = {{0,128,UCHAR_MAX},UCHAR_MAX};
    // //ColorAtValue red_290 = {{UCHAR_MAX,0,0},320};
    // violet_220.color=violet_220.color*((uint8_t)255);
    // std::vector<ColorAtValue<RGB<16>>> grad = {violet_220,blue_240,whiteblue_250,lightgreen_260,green_270,yellow_280,red_290};
    // //std::vector<ColorAtValue> grad = {blue_240,red_290};
    // for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path)){
    //     if(entry.is_regular_file())
    //         if(entry.path().has_extension() && (entry.path().extension()==".grib" || entry.path().extension()==".grb")){

    //             //std::cout<<entry.path()<<std::endl;
    //             GridData grid = extract(&extract_, entry.path().c_str(),&values,&count,&pos);
    //             Array_2D arr;
    //             count = 1;
    //             pos = 0;
    //             auto res = extract_ptr(&extract_,entry.path().c_str(),&count,&pos);
    //             arr.data = res.values;
    //             arr.nx = res.nx;
    //             arr.ny = res.ny;
    //             arr.dx = grid.dx;
    //             arr.dy = grid.dy;
    //             Array_2D result = bilinear_interpolation(arr,res.nx*4,res.ny*4);
    //             PngOutGray("temp_1990011512_X4xX4_gray.png",result.data,Size(result.nx,result.ny));
    //             PngOutRGBGradient<8>("temp_1990011512_X4xX4.png",result.data,Size(result.nx,result.ny),grad,true);
    //             //PngOut("temp_1990011512_X4xX4.png",result.data,result.nx,result.ny,grad);
    //         }
        
    // }
    return 0;
}