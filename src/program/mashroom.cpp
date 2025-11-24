#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cmd_parse/mashroom_parse.h>
#include <network/server.h>
#include <sys/config.h>
#include <sys/log_err.h>
#include "CLI/CLInavig.h"

namespace fs = std::filesystem;

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
            __Data__::FORMAT type_data = __Data__::FORMAT(text_to_data_type(key.data()));
            if(val.is_array() && type_data!=__Data__::FORMAT::UNDEF){
                for(const auto& filename:val.as_array())
                    if(type_data==__Data__::FORMAT::GRIB_v1 && filename.is_string())
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
        auto& files_seq = obj[format_name(format)].emplace_array();
        files_seq.emplace_back(filename.c_str());
    }
    assert(dat_file.is_open());
    std::cout<<"Writting to file: "<<__filename__()<<std::endl;
    dat_file<<val.as_object();
    dat_file.flush();
    std::cout<<val.as_object()<<std::endl;
    dat_file.close();
}

ErrorCode Mashroom::read_command(const std::vector<std::string>& argv){
    parse::Mashroom::instance().parse(argv);
    return ErrorCode::NONE;
}

bool Mashroom::read_command(){
    #if  defined(__unix__) || defined(__unix)
    read_command(boost::program_options::split_unix(std::string(CLIHandler::instance().input(std::string_view(">>")))));
    #elif defined(_WIN32)
    read_command(boost::program_options::split_unix(std::string(CLIHandler::instance().input(std::string_view(">>")))));
    #endif
    return true;
}
void Mashroom::collapse_server(bool wait_processes, uint16_t timeout_sec){
    if(server_)
        server_->collapse(wait_processes,timeout_sec);
    server_.reset();
}
void Mashroom::close_server(bool wait_processes, uint16_t timeout_sec){
    if(server_)
        server_->close(wait_processes,timeout_sec);
}
void Mashroom::shutdown_server(){
    if(server_)
        server_->close(false);
}
void Mashroom::deploy_server(){
    ErrorCode err;
    try{
        server_ = network::Server::make_instance(
        Application::config().current_server_setting().settings_);
    }
    catch(const std::exception& err){
        std::cout<<err.what()<<std::endl;
    }
    return;
}
void Mashroom::launch_server(){
    if(!server_)
        deploy_server();
    if(!server_){
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR, "server deploy and launching failure",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    if(server_->get_status()==network::server::Status::INACTIVE)
        server_->launch();
    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Server already launched",AT_ERROR_ACTION::CONTINUE);
}
