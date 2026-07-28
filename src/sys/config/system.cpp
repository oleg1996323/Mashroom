#include "config/system.h"
#include "sys/error.h"
#include "OsterLib/filesystem.h"
#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#endif

using namespace sys;

std::filesystem::path sys::get_app_data_dir() {
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

std::filesystem::path sys::get_config_dir(){
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

std::filesystem::path sys::get_cache_dir(){
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

std::string_view sys::user_config_filename() noexcept{
    return std::string_view("user.json");
}
std::string_view sys::system_config_filename() noexcept{
    return std::string_view("system.json");
}
std::string_view sys::network_config_filename() noexcept{
    return std::string_view("network.json");
}
std::string_view sys::history_filename() noexcept{
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

osterlib::ContextedError Config::session_logging_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return osterlib::ContextedError(
            mashroom::errc::create_directory_denied).with_field(
                "file",
                path.string());
    else {
        log_dir_ = path;
        return {};
    }
}
osterlib::ContextedError Config::configurations_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return osterlib::ContextedError(
            mashroom::errc::create_directory_denied).with_field(
                "file",
                path.string());
    else{
        config_dir_ = path;
        return {};
    }
}
osterlib::ContextedError Config::network_files_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return osterlib::ContextedError(
            mashroom::errc::create_directory_denied).with_field(
                "file",
                path.string());
    else{
        network_files_dir_ = path;
        return {};
    }
}
osterlib::ContextedError Config::cache_files_directory(const fs::path& path) noexcept{
    if(!directory_accessible(path))
        return osterlib::ContextedError(
            mashroom::errc::create_directory_denied).with_field(
                "file",
                path.string());
    else{
        cache_files_dir_ = path;
        return {};
    }
}

template<>
boost::json::value to_json(const sys::Config& val)
{
    boost::json::object result;
    result["log directory"]=val.session_logging_directory().c_str();
    result["configurations directory"]=val.configurations_directory().c_str();
    result["network files directory"]=val.network_files_directory().c_str();
    result["cache-files directory"]=val.cache_files_directory().c_str();
    return result;
}

template<>
std::expected<sys::Config,std::exception> 
        from_json(const boost::json::value& val)
{
    sys::Config result;
    if(val.is_object()){
        auto& obj = val.as_object();
        if(obj.contains("log directory")){
            if(obj.at("log directory").is_string())
               result.session_logging_directory(
                obj.at("log directory").as_string().c_str());
            else return std::unexpected(std::invalid_argument("not string value"));
        }
        if(obj.contains("configurations directory")){
            if(obj.at("configurations directory").is_string())
                result.session_logging_directory(
                    obj.at("configurations directory").as_string().c_str());
            else return std::unexpected(std::invalid_argument("not string value"));
        }
        if(obj.contains("network files directory")){
            if(obj.at("network files directory").is_string())
                result.session_logging_directory(
                obj.at("network files directory").as_string().c_str());
            else return std::unexpected(std::invalid_argument("not string value"));
        }
        if(obj.contains("cache-files directory")){
            if(obj.at("cache-files directory").is_string())
                result.session_logging_directory(
                    obj.at("cache-files directory").as_string().c_str());
            else return std::unexpected(std::invalid_argument("not string value"));
        }
    }
    else return std::unexpected(std::invalid_argument("not object type"));
    return result;
}