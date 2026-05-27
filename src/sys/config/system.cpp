#include "config/system.h"
#include "error_print.h"
#include "filesystem.h"

using namespace sys;

std::string_view user_config_filename() noexcept{
    return std::string_view("user.json");
}
std::string_view system_config_filename() noexcept{
    return std::string_view("system.json");
}
std::string_view network_config_filename() noexcept{
    return std::string_view("network.json");
}
std::string_view history_filename() noexcept{
    return std::string_view("clihist.txt");
}

const fs::path& Config::session_logging_directory() const noexcept{
    return log_dir_;
}
const fs::path& Config::configurations_directory() const noexcept{
    return config_dir_;
}
const fs::path& Config::network_files_directory() const noexcept{
    return network_files_dir_;
}
const fs::path& Config::system_files_directory() const noexcept{
    return system_files_dir_;
}

ErrorCode Config::set_session_logging_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else {
        log_dir_ = path;
        return ErrorCode::NONE;
    }
}
ErrorCode Config::set_configurations_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        config_dir_ = path;
        return ErrorCode::NONE;
    }
}
ErrorCode Config::set_network_files_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        network_files_dir_ = path;
        return ErrorCode::NONE;
    }
}
ErrorCode Config::set_system_files_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        system_files_dir_ = path;
        return ErrorCode::NONE;
    }
}