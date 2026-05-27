#pragma once
#include "sys/config.h"

namespace parse::server{
    ErrorCode add_config(const std::string& name, const fs::path& filename) noexcept{
        if(!app().config().server_config().add_from_file(name,filename))
            return  ErrorPrint::print_error(
                    ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "cannot add user configuration",AT_ERROR_ACTION::CONTINUE,
                    filename.c_str());
        else return ErrorCode::NONE;
    }
    ErrorCode remove_config(const std::string& name) noexcept{
        if(!Application::config().server_config().remove(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode modify_config(const std::string& name, const fs::path& filename) noexcept{
        if(!Application::config().server_config().modify_from_file(
                    name,filename))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",
                AT_ERROR_ACTION::CONTINUE,
                name);
        return ErrorCode::NONE;
    }
}