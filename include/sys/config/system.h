#pragma once
#include <filesystem>
#include <string_view>
#include "sys/error_code.h"

#ifndef __Mashroom_VERSION
    #define __Mashroom_VERSION
#endif

namespace sys{
    std::string_view user_config_filename() noexcept;
    std::string_view system_config_filename() noexcept;
    std::string_view network_config_filename() noexcept;
    std::string_view history_filename() noexcept;

    namespace fs = std::filesystem;
    std::filesystem::path get_app_data_dir();
    std::filesystem::path get_config_dir();
    std::filesystem::path get_cache_dir();
    
    class Config{
        public:
        ErrorCode set_session_logging_directory(const fs::path& path) noexcept;
        ErrorCode set_configurations_directory(const fs::path& path) noexcept;
        ErrorCode set_network_files_directory(const fs::path& path) noexcept;
        ErrorCode set_cache_files_directory(const fs::path& path) noexcept;
        const fs::path& session_logging_directory() const noexcept;
        const fs::path& configurations_directory() const noexcept;
        const fs::path& network_files_directory() const noexcept;
        const fs::path& cache_files_directory() const noexcept;
        static std::string default_log_directory() noexcept{
            return get_app_data_dir()/
                LOG_DIR;
        }
        static std::string default_config_directory() noexcept{
            return get_config_dir();
        }
        static std::string default_network_files_directory() noexcept{
            return get_cache_dir()/
                "network";
        }
        static std::string default_cache_files_directory() noexcept{
            return get_app_data_dir();
        }
        constexpr size_t version() const noexcept{
            return size_t(__Mashroom_VERSION);
        }
        private:
        fs::path log_dir_ = default_log_directory();
        fs::path config_dir_ = default_config_directory();
        fs::path network_files_dir_ = default_network_files_directory();
        fs::path cache_files_dir_ = default_cache_files_directory();
    };

    
}