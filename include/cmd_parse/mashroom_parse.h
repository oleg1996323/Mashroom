#pragma once
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/capitalize_parse.h"
#include "cmd_parse/extract_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/config_parse.h"
#include "cmd_parse/contains_parse.h"
#include "cmd_parse/functions.h"
#include "cmd_parse/server_parse.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"

namespace parse{
    namespace po = boost::program_options;
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        Extract::instance().parse(input);
    }
    ErrorCode capitalize_notifier(const std::vector<std::string>& input) noexcept{
        return Capitalize::instance().parse(input);
    }
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept{
        return Integrity::instance().parse(input);
    }
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept{
        return Contains::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        return Config::instance().parse(input);
    }
    ErrorCode save_notifier() noexcept{
        if(!hProgram->save())
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"saving failed",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }
    ErrorCode exit_notifier(const std::string& input) noexcept{
        if(input=="save")
            hProgram->save();
        exit(0);
    }

    class Mashroom:public BaseParser<parse::Mashroom>{
        Mashroom():BaseParser("Mashroom options:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
            ("extract",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = extract_notifier(items);
            }),"Extract specified data.")
            ("capitalize",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = capitalize_notifier(items);
            }),"Read specified files and register the contained data properties and data positions")
            ("integrity",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = integrity_notifier(items);
            }),"Check the integrity (dimensional and temporal) of capitalized data")
            ("contains",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = contains_notifier(items);
            }),"Check if capitalized data contains the data specified by properties")
            ("config",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = config_notifier(items);
            }),"Permits to configure the program or server")
            ("save","Save the current instance")
            ("help",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
                err_ = Contains::instance().parse(items);
            }),"Show help")
            ("exit",po::value<std::string>()->implicit_value("save")->notifier([this](const std::string& item){
                err_ = exit_notifier(item);
            }),"Exit from program");
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            if(err_!=ErrorCode::NONE)
                return err_;
            else if(vm.contains("save") && vm.size()==1){
                err_ = save_notifier();
                return err_;
            }
            else{
                err_=try_notify(vm);
                return err_;
            }
        }
    };
}