#include "config/system.h"
#include "error_print.h"
#include "filesystem.h"
#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#endif

using namespace sys;

std::filesystem::path get_app_data_dir() {
    const char* app_name = "Mashroom";
    #ifdef _WIN32
        wchar_t* path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
            std::filesystem::path result = path;
            CoTaskMemFree(path);
            return result / app_name;
        }
        // fallback
        return std::getenv("LOCALAPPDATA") / app_name;
    #else
        const char* xdg_data = std::getenv("XDG_DATA_HOME");
        if (xdg_data) return std::filesystem::path(xdg_data) / app_name;
        return std::filesystem::path(std::getenv("HOME"))/".local/share" / app_name;
    #endif
}

std::filesystem::path get_config_dir(){
    const char* app_name = "Mashroom";
    if (const char* env_path = std::getenv("XDG_CONFIG_HOME")){
        if (env_path[0] != '\0') { // Проверка, что переменная не пустая
            return fs::path(env_path)/app_name;
        }
    }
    if (const char* home = std::getenv("HOME")){
        return std::filesystem::path(home)/".config"/app_name;
    }
    return {};
}

std::filesystem::path get_cache_dir(){
    const char* app_name = "Mashroom";
    if (const char* env_path = std::getenv("XDG_CACHE_HOME")){
        if (env_path[0] != '\0') { // Проверка, что переменная не пустая
            return fs::path(env_path)/app_name;
        }
    }
    if (const char* home = std::getenv("HOME")){
        return std::filesystem::path(home)/".cache"/app_name;
    }
    return {};
}

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
const fs::path& Config::cache_files_directory() const noexcept{
    return cache_files_dir_;
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
ErrorCode Config::set_cache_files_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,path.string());
    else{
        cache_files_dir_ = path;
        return ErrorCode::NONE;
    }
}