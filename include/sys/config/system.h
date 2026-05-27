#pragma once
#include <filesystem>
#include <string_view>
#include "sys/error_code.h"

namespace sys{
    std::string_view user_config_filename() noexcept;
    std::string_view system_config_filename() noexcept;
    std::string_view network_config_filename() noexcept;
    std::string_view history_filename() noexcept;

    namespace fs = std::filesystem;
    static fs::path get_config_dir(){
        fs::path res = fs::path(getenv("HOME"));
        res/=CONFIG_MASHROOM_DIR_REL;
        return res;
    }
    
    class Config{
        public:
        ErrorCode set_session_logging_directory(const fs::path& path) noexcept;
        ErrorCode set_configurations_directory(const fs::path& path) noexcept;
        ErrorCode set_network_files_directory(const fs::path& path) noexcept;
        ErrorCode set_system_files_directory(const fs::path& path) noexcept;
        const fs::path& session_logging_directory() const noexcept;
        const fs::path& configurations_directory() const noexcept;
        const fs::path& network_files_directory() const noexcept;
        const fs::path& system_files_directory() const noexcept;
        private:
        fs::path log_dir_ = LOG_DIR;
        fs::path config_dir_ = get_config_dir();
        fs::path network_files_dir_ = get_config_dir();
        fs::path system_files_dir_ = fs::current_path();
    };

    
}