#include "cmd_parse/check_parse.h"
#include "check.h"

void check_parse(std::vector<std::string_view> input){
    unsigned int cpus = std::thread::hardware_concurrency();
    fs::path path;
    fs::path out;
    Date from_d;
    Date to_d;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt(input[i])){
            case Command::THREADS:{
                unsigned int tmp = from_chars<unsigned int>(input[++i]);
                if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
                    cpus = tmp;
            }
            case Command::IN_PATH:{
                path = input[++i];
                if(!fs::is_directory(path))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,path.c_str());
            }
            case Command::OUT_PATH:{
                out = input[++i];
                if(!fs::is_directory(out))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,path.c_str());
            }
            case Command::DATE_FROM:{
                from_d = get_date_from_token(input[++i]);
            }
            case Command::DATE_TO:{
                to_d = get_date_from_token(input[++i]);
            }
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::ABORT,path.c_str());
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
    if(missing_files(path,out,cpus,1d/1/1991))
        std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
    else std::cout<<"No missing files detected"<<std::endl;
}