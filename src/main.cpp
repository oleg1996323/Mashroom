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

int main(int argc, char* argv[]){
    std::cout << "Command-line arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    unsigned int cpus = std::thread::hardware_concurrency();
    std::filesystem::path path;
    for(int i = 1;i<argc;++i){
        if(strcmp(argv[i],"-j")==0){
            long tmp = std::stol(argv[++i]);
            if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
                cpus = tmp;
        }
        else if(strcmp(argv[i],"-p")==0){
            path = argv[++i];
            if(!std::filesystem::is_directory(path)){
                std::cout<<"Set path=env ${HOME}"<<std::endl;
                path = getenv("HOME");
            }
        }
    }
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
    if(missing_files(path,cpus,1d/1/1991))
        std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
    else std::cout<<"No missing files detected"<<std::endl;
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