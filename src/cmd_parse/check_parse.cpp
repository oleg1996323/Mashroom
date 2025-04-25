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
    std::chrono::system_clock::time_point from_d;
    std::chrono::system_clock::time_point to_d;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return;
                hCheck.set_using_processor_cores(tmp_proc_num.value());
                break;
            }
            case Command::IN_PATH:{
                ErrorCode err = hCheck.set_checking_directory(input.at(i));
                if(err!=ErrorCode::NONE)
                    return;
                break;
            }
            case Command::OUT_PATH:{
                    std::vector<std::string_view> tokens = split(input.at(i),":");
                    if(tokens.size()!=2){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input.at(i));
                        return;
                    }
                    if(tokens.at(0)=="dir"){
                        ErrorCode err = hCheck.set_destination_directory(tokens.at(1));
                        if(err!=ErrorCode::NONE)
                            return;
                    }
                    else if(tokens.at(0)=="ip"){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                        return;
                    }
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::CONTINUE,input.at(i));
                        return;
                    }
                    break;
            }
            case Command::DATE_FROM:{
                auto tmp_date = get_date_from_token(input.at(i));
                if(!tmp_date.has_value())
                    return;
                from_d = tmp_date.value();
            }
            case Command::DATE_TO:{
                auto tmp_date = get_date_from_token(input.at(i));
                if(!tmp_date.has_value())
                    return;
                to_d = tmp_date.value();
            }
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::CONTINUE,path.c_str());
                return;
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
    if(hCheck.execute())
        std::cout<<"Missing files are placed to file "<<path/"missing_files.txt"<<std::endl;
    else std::cout<<"No missing files detected"<<std::endl;
}

std::vector<std::string_view> commands_from_check_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    std::vector<std::string_view> commands;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return {};
                commands.push_back(input.at(i));
                break;
            }
            case Command::IN_PATH:{
                if(!fs::is_directory(input.at(i))){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                break;
            }
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input.at(i),":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1))){
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,tokens.at(1));
                            return {};
                        }
                    }
                    commands.push_back(input.at(i));
                }
                else if(tokens.at(0)=="ip"){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    return {};
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                break;
            }
            case Command::DATE_FROM:{
                if(!get_date_from_token(input.at(i)).has_value())
                    return {};
                commands.push_back(input.at(i));
            }
            case Command::DATE_TO:{
                if(!get_date_from_token(input.at(i)).has_value())
                    return {};
                commands.push_back(input.at(i));
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::CONTINUE);
                return {};
            }
        }
    }
    if(commands.empty()){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero args for check mode",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    return commands;
}