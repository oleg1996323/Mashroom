#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cmd_parse/server_parse.h>
#include <cmd_parse/capitalize_parse.h>
#include <cmd_parse/integrity_parse.h>
#include <cmd_parse/extract_parse.h>
#include <cmd_parse/config_parse.h>
#include <cmd_parse/cmd_translator.h>
#include <cmd_parse/functions.h>
#include <network/server.h>

namespace fs = std::filesystem;

void Mashroom::__read_initial_data_file__(){
    using namespace boost;
    if(!fs::exists(data_dir_/mashroom_data_info)){
        dat_file.open(__filename__(),std::ios::trunc|std::ios::out);
        return;
    }
    else
        dat_file.open(__filename__(),std::ios::in|std::ios::out);

    if(!dat_file.is_open()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Mashroom module internal error",AT_ERROR_ACTION::CONTINUE);
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(data_dir_/mashroom_data_info).c_str());
    }
    json::stream_parser parser;
    json::error_code err_code;
    std::array<char,1024*4> buffer;
    while(dat_file.good()){
        dat_file.read(buffer.data(),buffer.size());
        parser.write(buffer.data(),dat_file.gcount(),err_code);
        if(err_code)
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,""s+(__filename__()).c_str()+" reading error",AT_ERROR_ACTION::ABORT);
    }
    if(!parser.done())
        return;
    else
        parser.finish();
    json::value root = parser.release();
    if(root.is_object())
        for(const auto& [key,val]:root.as_object()){
            DataTypeInfo type_data = text_to_data_type(key.data());
            if(val.is_array() && type_data!=DataTypeInfo::Undef){
                for(const auto& filename:val.as_array())
                    if(type_data==DataTypeInfo::Grib && filename.is_string())
                        data_files_.insert(filename.as_string().c_str());
            }
        }
    for(const fs::path& filename:data_files_){
        data_.read(filename);
    }
}
using namespace std::string_literals;
void Mashroom::__write_initial_data_file__(){
    using namespace boost;
    if(!dat_file.is_open())
        if(!fs::exists(__filename__())){
            dat_file.open(__filename__(),std::ios::out);
            if(!dat_file.is_open()){
                ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,(__filename__()).c_str());
                if(!fs::exists(__crash_dir__()))
                    if(!fs::create_directories(fs::path(std::getenv("HOME"))/"mashroom_crash"))
                        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Data file saving error",AT_ERROR_ACTION::ABORT);
                    dat_file.open(__crash_path__());
                    if(!dat_file.is_open())
                        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Data file saving error",AT_ERROR_ACTION::ABORT);
                    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Data file saving error.\nThe data file will be saved to \""s+
                        __crash_path__().c_str()+"\"",AT_ERROR_ACTION::ABORT);
            }
        }
    json::value val;
    auto& obj = val.emplace_object();
    for(const auto& filename:data_.written_files()){
        auto& files_seq = obj[data_type_to_text(extension_to_type(filename.extension().c_str()))].emplace_array();
        files_seq.emplace_back(filename.c_str());
    }
    assert(dat_file.is_open());
    std::cout<<"Writting to file: "<<__filename__()<<std::endl;
    dat_file<<val.as_object();
    dat_file.flush();
    std::cout<<val.as_object()<<std::endl;
}

ErrorCode Mashroom::read_command(const std::vector<std::string_view>& argv){
    if(argv.size()<1){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Haven't arguments",AT_ERROR_ACTION::CONTINUE,"");
        return ErrorCode::COMMAND_INPUT_X1_ERROR;
    }
    std::vector<std::string_view> arguments;
    ErrorCode err;
    if(argv.size()>1)
        arguments.append_range(std::vector<std::string_view>(argv.begin()+1,argv.end()));
    {
        switch(translate_from_txt<translate::token::ModeArgs>(argv.at(0))){
            case translate::token::ModeArgs::CAPITALIZE:
                if(arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_FEW_ARGUMENTS;
                }
                return capitalize_parse(arguments);
                break;
            case translate::token::ModeArgs::EXTRACT:
                if(arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_FEW_ARGUMENTS;
                }
                return extract_parse(arguments);
                break;
            case translate::token::ModeArgs::INTEGRITY:
                if(arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_FEW_ARGUMENTS;
                }
                return integrity_parse(arguments);
                break;
            case translate::token::ModeArgs::CONTAINS:
                if(arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_FEW_ARGUMENTS;
                }
                //TODO
                break;
            case translate::token::ModeArgs::CONFIG:
                if(arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_FEW_ARGUMENTS;
                }
                return config_parse(arguments);
                break;
            case translate::token::ModeArgs::HELP:
                if(arguments.size()>1){
                    ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_MANY_ARGUMENTS;
                }
                else if(arguments.size()==1){
                    //help(*mode*)
                }
                else
                    help();
                break;
            case translate::token::ModeArgs::SAVE:
                if(!arguments.empty()){
                    ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::TO_MANY_ARGUMENTS;
                }
                if(!hProgram || !hProgram->save()){
                    ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Nothing to save",AT_ERROR_ACTION::CONTINUE,argv.at(0));
                    return ErrorCode::IGNORING_VALUE_X1;
                }
                break;
            case translate::token::ModeArgs::EXIT:
                if(argv.size()>1){
                    ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE,argv.at(1));
                    return ErrorCode::TO_MANY_ARGUMENTS;
                }
                exit(0);
                break;
            case translate::token::ModeArgs::SERVER:
                if(arguments.empty())
                    return ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"server mode",AT_ERROR_ACTION::CONTINUE);
                return server_parse(arguments);
                break;
            default:{
                if(Application::config().has_config_name(argv.at(0))){
                    if(argv.size()>1){
                        ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE,argv.at(1));
                        return ErrorCode::TO_MANY_ARGUMENTS;
                    }
                    return read_command(Application::config().get_user_config(argv.at(0))|std::ranges::views::transform([](auto& str){
                        return std::string_view(str);
                    })|std::ranges::to<std::vector<std::string_view>>());
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,argv.at(0));
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
            }
        }
    }  
    return ErrorCode::NONE;
}

bool Mashroom::read_command(std::istream& stream){
    std::string line;
    std::cout<<">> ";
    std::cout.flush();
    if (!std::getline(stream, line))
        return false;
    std::string_view view(line);
    std::vector<std::string_view> commands=split(view," ");
    read_command(commands);
    return true;
}
void Mashroom::collapse_server(bool wait_processes){
    Server::collapse(server_.get(),wait_processes);
}
void Mashroom::close_server(bool wait_processes){
    if(server_)
        server_->close_connections(wait_processes);
}
void Mashroom::shutdown_server(bool wait_processes){
    if(server_)
        server_->shutdown(wait_processes);
}
void Mashroom::deploy_server(){
    ErrorCode err;
    server_ = server::Server::make_instance(err);
    return;
}
void Mashroom::launch_server(){
    if(!server_)
        deploy_server();
    if(!server_){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR, "server deploy and launching failure",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    server_->launch();
}
