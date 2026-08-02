#include "program/mashroom.h"
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "cmd_parse/mashroom_parse.h"
#include "web/server.h"
#include "sys/config.h"
#include "OsterLib/CLI/CLInavig.h"
#include <CLI/CLI.hpp>
#include <boost/program_options.hpp>

namespace fs = std::filesystem;

CLI::App& Mashroom::command_line() noexcept{
    static std::unique_ptr<CLI::App> cli;
    if(!cli)
        cli = std::make_unique<CLI::App>("Geoinformational data indexer/extractor","Mashroom");
    return *cli;
}

osterlib::ContextedError Mashroom::__read_initial_data_file__() noexcept{
    using namespace boost;
    std::fstream dat_file;
    if(!fs::exists(__filename__())){
        std::cout<<"Creating and openning file: "<<__filename__()<<std::endl;
        dat_file.open(__filename__(),std::ios::trunc|std::ios::out);
        if(!dat_file.is_open())
            return osterlib::ContextedError(mashroom::errc::file_permission_denied)
                .with_field("at","read initial data file")
                .with_field("file",__filename__().c_str());
        else return {};
    }
    else{
        std::cout<<"Openning file: "<<__filename__()<<std::endl;
        dat_file.open(__filename__(),std::ios::in|std::ios::out);
    }

    if(!dat_file.is_open()){
        osterlib::ContextedError ctx_err(mashroom::errc::internal_error,"openning data file error");
        ctx_err.with_field("at","Mashroom initialization")
        .with_field("file",__filename__().c_str());
        std::cerr<<ctx_err.what()<<std::endl;
        exit((int)mashroom::errc::internal_error);
    }
    json::stream_parser parser;
    json::error_code err_code;
    std::array<char,1024*4> buffer;
    while(dat_file.good()){
        dat_file.read(buffer.data(),buffer.size());
        parser.write(buffer.data(),dat_file.gcount(),err_code);
        if(err_code){
            osterlib::ContextedError ctx_err(mashroom::errc::file_reading_error);
            ctx_err.with_field("at","Mashroom initialization")
            .with_field("file",__filename__().c_str());
            std::cerr<<ctx_err.what()<<std::endl;
            exit((int)mashroom::errc::file_reading_error);
        }
    }
    if(!parser.done()){
        osterlib::ContextedError ctx_err(mashroom::errc::file_reading_error);
        ctx_err.with_field("at","Mashroom initialization")
        .with_field("file",__filename__().c_str());
        std::cerr<<ctx_err.what()<<std::endl;
        return ctx_err;
    }
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
    return {};
}
using namespace std::string_literals;
osterlib::ContextedError Mashroom::__write_initial_data_file__() noexcept{
    using namespace boost;
    std::fstream dat_file(__filename__(),std::fstream::out | std::fstream::trunc);
    if(!dat_file.is_open())
        if(!fs::exists(__filename__())){
            dat_file.open(__filename__(),std::ios::out);
            if(!dat_file.is_open()){
                osterlib::ContextedError ctx_err(mashroom::errc::file_permission_denied,"openning data file error");
                ctx_err.with_field("at","Mashroom saving")
                .with_field("file",__filename__().c_str());
                std::cerr<<ctx_err.what()<<std::endl;
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
std::error_code Mashroom::read_command(std::vector<std::string>&& argv){
    //needed reversing (see parse(...) functions in CLI11)
    std::reverse(argv.begin(),argv.end());
    try {
        Mashroom::command_line().parse(std::move(argv));
    }
    catch(const CLI::CallForHelp& help){
        std::cout<<command_line().help()<<std::endl;
    }catch(const CLI::CallForAllHelp& help){
        std::cout<<command_line().help("",CLI::AppFormatMode::All)<<std::endl;
    }catch(const CLI::CallForVersion& version){
        
    }
    catch (const CLI::ParseError &e) {
        std::cout<<e.what()<<std::endl;
        return std::make_error_code(mashroom::errc::command_input_error);
    }
    catch(const osterlib::ContextedError& err){
        std::cout<<err.what()<<std::endl;
        return err.code();
    }
    return {};
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
