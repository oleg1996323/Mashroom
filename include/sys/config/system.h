#pragma once
#include <filesystem>

constexpr const char* uc_filename = "config.json";
constexpr const char* sc_filename = "server.json";

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
    };
}