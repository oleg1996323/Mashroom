#pragma once
#include <filesystem>
#include <string_view>

constexpr std::string_view uc_filename = std::string_view("config.json");
constexpr std::string_view sc_filename = std::string_view("server.json");
constexpr std::string_view cli_history_filename = std::string_view("clihist.txt");

namespace sys{
    namespace fs = std::filesystem;
    static fs::path get_config_dir(){
        fs::path res = fs::path(getenv("HOME"));
        res/=CONFIG_MASHROOM_DIR_REL;
        return res;
    }

    struct Settings{
        fs::path log_dir = fs::current_path();
        fs::path config_dir = get_config_dir();
        fs::path server_config_dir = get_config_dir();
        fs::path system_files_dir = fs::current_path();
    };

    
}