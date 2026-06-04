#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "proc/index.h"
#include "sys/application.h"
#include "sys/config.h"
#include <boost/program_options.hpp>

namespace parse{
    ErrorCode add_user_config(const std::string& name, const fs::path& filename) noexcept{
        if(!app().config().user_config().add_from_file(name,filename))
            return  ErrorPrint::print_error(
                    ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "cannot add user configuration",AT_ERROR_ACTION::CONTINUE,
                    filename.c_str());
        else return ErrorCode::NONE;
    }
    ErrorCode remove_user_config_notifier(const std::string& name) noexcept{
        if(!Application::config().user_config().remove(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode modify_user_config_notifier(const std::string& name, const fs::path& filename) noexcept{
        if(!Application::config().user_config().modify_from_file(
                    name,filename))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",
                AT_ERROR_ACTION::CONTINUE,
                name);
        return ErrorCode::NONE;
    }
    

    ProgramConfig::ProgramConfig():AbstractCLIParser("Config options:"){}

    void ProgramConfig::init() noexcept{
        add_options_instances("system",po::value<std::vector<std::string>>(),"Configure the system settings",SystemConfig::instance());
        add_options_instances("user",po::value<std::vector<std::string>>(),"Configure the user settings",UserConfig::instance());
        add_options_instances("network",po::value<std::vector<std::string>>(),"Configure the network settings",NetworkConfig::instance());
        define_uniques();
    }
    ErrorCode ProgramConfig::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        return try_notify(vm);
    }
}