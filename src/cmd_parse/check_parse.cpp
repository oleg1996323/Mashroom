#include "cmd_parse/check_parse.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "check.h"
#include "cmd_parse/functions.h"

void check_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    Check hCheck;
    fs::path path;
    fs::path out;
    Date from_d;
    Date to_d;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:
                hCheck.set_using_processor_cores(from_chars<unsigned int>(input.at(i)));
                break;
            case Command::IN_PATH:
                hCheck.set_checking_directory(input.at(i));
                break;
            case Command::OUT_PATH:{
                if(!fs::is_directory(out))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,path.c_str());

                    std::vector<std::string_view> tokens = split(input.at(i),":");
                    if(tokens.size()!=2)
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input.at(i));
                    if(tokens.at(0)=="dir"){
                        if(!fs::is_directory(tokens.at(1))){
                            if(!fs::create_directory(tokens.at(1)))
                                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,tokens.at(1));
                        }
                        out = tokens.at(1);
                    }
                    else if(tokens.at(0)=="ip")
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::ABORT,input.at(i));
                    break;
            }
            case Command::DATE_FROM:{
                from_d = get_date_from_token(input.at(i));
            }
            case Command::DATE_TO:{
                to_d = get_date_from_token(input.at(i));
            }
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::ABORT,path.c_str());
        }
    }
    // {
    //     // Отключаем канонический режим и эхо (для чтения ответа терминала)
    //     termios oldt;
    //     tcgetattr(STDIN_FILENO, &oldt);
    //     termios newt = oldt;
    //     newt.c_lflag &= ~(ICANON | ECHO);
    //     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    //     int row,col;
    //     if(getCursorPosition(row,col))
    //         progress_line = row;
    //     else exit(1);
    // }
    if(missing_files(path,out,cpus,1d/1/1991))
        std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
    else std::cout<<"No missing files detected"<<std::endl;
}

std::vector<std::string_view> commands_from_check_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    std::vector<std::string_view> commands;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                commands.push_back(input.at(i));
                break;
            }
            case Command::IN_PATH:{
                if(!fs::is_directory(input.at(i)))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,input.at(i));
                break;
            }
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input.at(i),":");
                if(tokens.size()!=2)
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input.at(i));
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1)))
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,tokens.at(1));
                    }
                    commands.push_back(input.at(i));
                }
                else if(tokens.at(0)=="ip")
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::ABORT,input.at(i));
                break;
            }
            case Command::DATE_FROM:{
                Date tmp = get_date_from_token(input.at(i));
                if(is_correct_date(&tmp))
                    commands.push_back(input.at(i));
                else
                    ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::ABORT,input.at(i));
            }
            case Command::DATE_TO:{
                Date tmp = get_date_from_token(input.at(i));
                if(is_correct_date(&tmp))
                    commands.push_back(input.at(i));
                else
                    ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::ABORT,input.at(i));
            }
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::ABORT);
        }
    }
    if(commands.empty())
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero args for check mode",AT_ERROR_ACTION::ABORT);
    return commands;
}