#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cmd_parse/mashroom_parse.h>
#include <network/server.h>
#include <sys/config.h>
#include <sys/log_err.h>

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
            __Data__::FORMAT type_data = __Data__::FORMAT(text_to_data_type(key.data()));
            if(val.is_array() && type_data!=__Data__::FORMAT::UNDEF){
                for(const auto& filename:val.as_array())
                    if(type_data==__Data__::FORMAT::GRIB && filename.is_string())
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

ErrorCode Mashroom::read_command(const std::vector<std::string>& argv){
    parse::Mashroom::instance().parse(argv);
    return ErrorCode::NONE;
}

bool Mashroom::read_command(std::istream& stream){
    std::string line;
    std::cout<<">> ";
    std::cout.flush();
    if (!std::getline(stream, line))
        return false;
    std::vector<std::string> commands=split<std::string>(line," ");
    read_command(commands);
    return true;
}
void Mashroom::collapse_server(bool wait_processes){
    if(server_)
        server_->close_connections(wait_processes);
    server_.reset();
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
    server_ = network::Server::make_instance(err);
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
