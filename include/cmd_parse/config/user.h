#pragma once
#include "sys/config.h"

namespace parse::user{
    ErrorCode add_config(const std::string& name, const fs::path& filename) noexcept{
        if(!app().config().user_config().add_from_file(name,filename))
            return  ErrorPrint::print_error(
                    ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "cannot add user configuration",AT_ERROR_ACTION::CONTINUE,
                    filename.c_str());
        else return ErrorCode::NONE;
    }
    ErrorCode remove_config(const std::string& name) noexcept{
        if(!Application::config().user_config().remove(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode modify_config(const std::string& name, const fs::path& filename) noexcept{
        if(!Application::config().user_config().modify_from_file(
                    name,filename))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",
                AT_ERROR_ACTION::CONTINUE,
                name);
        return ErrorCode::NONE;
    }
}