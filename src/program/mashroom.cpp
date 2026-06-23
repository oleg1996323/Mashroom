#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "cmd_parse/mashroom_parse.h"
#include "web/server.h"
#include "sys/config.h"
#include "sys/log_err.h"
#include "CLI/CLInavig.h"
#include <CLI/CLI.hpp>
#include <boost/program_options.hpp>

namespace fs = std::filesystem;

CLI::App& Mashroom::command_line() noexcept{
    static std::unique_ptr<CLI::App> cli;
    if(!cli)
        cli = std::make_unique<CLI::App>("Geoinformational data indexer/extractor","Mashroom");
    return *cli;
}

void Mashroom::__read_initial_data_file__(){
    using namespace boost;
    std::fstream dat_file;
    if(!fs::exists(__filename__())){
        std::cout<<"Creating and openning file: "<<__filename__()<<std::endl;
        dat_file.open(__filename__(),std::ios::trunc|std::ios::out);
        return;
    }
    else{
        std::cout<<"Openning file: "<<__filename__()<<std::endl;
        dat_file.open(__filename__(),std::ios::in|std::ios::out);
    }

    if(!dat_file.is_open()){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Mashroom module internal error",AT_ERROR_ACTION::CONTINUE);
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(__filename__()).c_str());
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
            Data_f fmt_data;
            if(auto fmt_tmp = to_data_format_token(key);fmt_tmp.has_value())
                fmt_data = fmt_tmp.value();
            if(val.is_array()){
                for(const auto& filename:val.as_array())
                    if(fmt_data==Data_f::GRIB_v1 && filename.is_string())
                        data_files_.insert(filename.as_string().c_str());
            }
        }
    for(const fs::path& filename:data_files_){
        data_.read(filename);
    }
    dat_file.close();
}
using namespace std::string_literals;
void Mashroom::__write_initial_data_file__(){
    using namespace boost;
    std::fstream dat_file(__filename__(),std::fstream::out | std::fstream::trunc);
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
    for(const auto& [format,filename]:data_.written_files()){
        auto& files_seq = obj[to_data_format_name(format)].emplace_array();
        files_seq.emplace_back(filename.c_str());
    }
    assert(dat_file.is_open());
    std::cout<<"Writting to file: "<<__filename__()<<std::endl;
    dat_file<<val.as_object();
    dat_file.flush();
    std::cout<<val.as_object()<<std::endl;
    dat_file.close();
}
ErrorCode Mashroom::read_command(std::vector<std::string>&& argv){
    try {
        Mashroom::command_line().parse(std::move(argv));
    }
    catch(const CLI::CallForHelp& help){
        std::cout<<command_line().help()<<std::endl;
    } catch (const CLI::ParseError &e) {
        std::cout<<e.what()<<std::endl;
        return ErrorCode::COMMAND_INPUT_X1_ERROR;
    }
    return ErrorCode::NONE;
}
bool Mashroom::read_command(){
    try{
        read_command(boost::program_options::split_unix(std::string(CLIHandler::instance().input(std::string_view(">>")))));
    }
    catch(const std::exception& e){
        std::cout<<e.what()<<std::endl;
    }
    return true;
}
