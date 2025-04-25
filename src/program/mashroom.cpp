#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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

void Mashroom::read_command(const std::vector<std::string_view>& argv){
    std::cout << "Command-line arguments:" << std::endl;
    if(argv.size()<1){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero arguments",AT_ERROR_ACTION::CONTINUE,"");
        return;
    }

    for (size_t i = 0;i<argv.size();++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }
    MODE mode = MODE::NONE;
    switch(translate_from_txt<translate::token::ModeArgs>(argv.at(0))){
        case translate::token::ModeArgs::EXTRACT:
            mode = MODE::EXTRACT;
            break;
        case translate::token::ModeArgs::CAPITALIZE:
            mode = MODE::CAPITALIZE;
            break;
        case translate::token::ModeArgs::CHECK:
            mode = MODE::CHECK_ALL_IN_PERIOD;
            break;
        case translate::token::ModeArgs::CONFIG:
            mode = MODE::CONFIG;
            break;
        case translate::token::ModeArgs::HELP:
            mode = MODE::HELP;
            for(int i=1;i<argv.size();++i)
                ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,argv.at(i));
            help();
            return;
            break;
        case translate::token::ModeArgs::EXIT:
            mode = MODE::EXIT;
            if(argv.size()>1){
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,argv.at(1));
                return;
            }
            exit(0);
            break;
        case translate::token::ModeArgs::SAVE:
            if(!hProgram || !hProgram->save())
                ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Nothing to save",AT_ERROR_ACTION::CONTINUE,argv.at(0));
            return;
            break;
        default:
            if(Application::config().has_config_name(argv.at(0))){
                for(int i=1;i<argv.size();++i)
                    ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,argv.at(i));
                return read_command(Application::config().get_user_config(argv.at(0))|std::ranges::views::transform([](auto& str){
                    return std::string_view(str);
                })|std::ranges::to<std::vector<std::string_view>>());
            }
            else{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,argv.at(0));
                return;
            }
    }
    
    std::vector<std::string_view> args(argv.begin()+1,argv.end());
    {
        switch(mode){
            case MODE::CAPITALIZE:
                capitalize_parse(args);
                break;
            case MODE::EXTRACT:
                extract_parse(args);
                break;
            case MODE::CHECK_ALL_IN_PERIOD:
                check_parse(args);
                break;
            case MODE::CONFIG:
                config_parse(args);
                break;
            case MODE::HELP:
                help();
                break;
            case MODE::EXIT:
                exit(0);
                break;
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missing mode operation 1st argument",AT_ERROR_ACTION::CONTINUE);
                return;
        }
    }
    //for(int i = 1;i<argv.size();++i){
        //date from for extraction
        //input separated by ':' with first tokens ('h','d','m','y'),integer values
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
        
        //input integer or float value with '.' separation

        //input integer or float value with '.' separation
        
        
        
        // else if(strcmp(argv[i],"-send")==0){
        //     ++i;
        //     char* arg = argv[i];
        //     char* tokens;
        //     char* type = strtok_r(arg,":",&tokens);
        //     if(strcmp(type,"dir")==0){
        //         out = strtok_r(NULL,":",&tokens);
        //         if(!std::filesystem::is_directory(out)){
        //             if(!std::filesystem::create_directory(out)){
        //                 std::cout<<out<<" is not a directory. Abort.";
        //                 exit(1);
        //             }
        //         }
        //     }
        //     else if(strcmp(type,"ip")==0 && mode==MODE::EXTRACT){
        //         out = strtok_r(NULL,":",&tokens);
        //     }
        //     else{
        //         std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
        //         exit(1);
        //     }
        // }
        // else{
        //     std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
        //     exit(1);
        // }
    //}    
    return;
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