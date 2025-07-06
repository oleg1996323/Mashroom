#pragma once
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/capitalize_parse.h"
#include "cmd_parse/extract_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/config_parse.h"
#include "cmd_parse/contains_parse.h"
#include "cmd_parse/functions.h"
#include "cmd_parse/server_parse.h"
#include <boost/program_options.hpp>

namespace parse{
    namespace po = boost::program_options;
    class Mashroom:BaseParser<parse::Mashroom>{
        Mashroom():BaseParser("Mashroom options:"){}

        virtual void __init__() noexcept override final{
            descriptor_.add_options()
            ("extract","Extract specified data.")
            ("capitalize","Read specified files and register the contained data properties and data positions")
            ("integrity","Check the integrity (dimensional and temporal) of capitalized data")
            ("contains","Check if capitalized data contains the data specified by properties")
            ("config","Permits to configure the program or server")
            ("save","Save the current instance")
            ("help","Show help")
            ("exit","Exit from program");
            define_uniques();
        }
        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final{
                        po::variables_map vm;
            ErrorCode err_ = ErrorCode::NONE;
            auto parse_result = try_parse(instance().descriptor(),args);
            if(!parse_result.has_value())
                return parse_result.error();
            po::variables_map vm;
            po::store(parse_result.value(),vm);
            std::vector<std::string> tokens = po::collect_unrecognized(parse_result.value().options,po::collect_unrecognized_mode::include_positional);
            auto dublicate = contains_unique_value(tokens.begin(),tokens.end(),unique_values_.begin(),unique_values_.end());
            if(dublicate!=unique_values_.end())
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"must be unique",AT_ERROR_ACTION::CONTINUE,*dublicate);
            if(vm.contains("extract")){

            }
            else if(vm.contains("capitalize")){
            }
            else if(vm.contains("integrity")){

            }
            else if(vm.contains("contains")){
                
            }
            else if(vm.contains("config")){
                
            }
            else if(vm.contains("save")){
                
            }
            else if(vm.contains("help")){
                
            }
            else if(vm.contains("exit")){
                
            }
            else{
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,tokens.front());
            }
        }
        public:
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{

        }
    };
}