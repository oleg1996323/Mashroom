#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "proc/index.h"
#include "sys/application.h"
#include "sys/config.h"

namespace parse{
    SystemConfig::SystemConfig(CLI::App* app):
    app_(app){
        app_->require_option();
        CLI::Option* log_dir_=app_
        ->add_option("--log-dir",log_dir_val_,
        "the directory where log files will be placed")
        ->default_val(::app().config().system_config().
            session_logging_directory())
        ->capture_default_str();
        CLI::Option* config_dir_=app_
        ->add_option("--config-dir",config_dir_val_,
        "configurations directory")
        ->default_val(::app().config().system_config().
            configurations_directory().string())
        ->capture_default_str();
        CLI::Option* cache_files_dir_=app_
        ->add_option("--cache-dir",cache_files_dir_val_,
        "cache files directory")
        ->default_val(::app().config().system_config().
            cache_files_directory())
        ->capture_default_str();
        CLI::Option* network_files_dir_=app_
        ->add_option("--network-dir",network_files_dir_val_,
        "network files directory")
        ->default_val(::app().config().system_config().
            network_files_directory())
        ->capture_default_str();
        app_->callback([this](){
            this->execute();
        });
    }
    void SystemConfig::execute(){
        if(app_->count("--log-dir")){
            ::app().config().system_config().
            session_logging_directory(log_dir_val_);
        }
        if(app_->count("--config-dir")){
            ::app().config().system_config().
            configurations_directory(config_dir_val_);
        }
        if(app_->count("--cache-dir")){
            ::app().config().system_config().
            cache_files_directory(cache_files_dir_val_);
        }
        if(app_->count("--network-dir")){
            ::app().config().system_config().
            network_files_directory(network_files_dir_val_);
        }
    }

    NetworkConfig::NetworkConfig(CLI::App* app):
    app_(app)
    {
        app_->require_subcommand(1);
        CLI::App* server_ = app_->add_subcommand("server","server configuration");
        static std::unique_ptr<ServerConfig> server = std::make_unique<ServerConfig>(server_);
        CLI::App* client_ = app_->add_subcommand("client","client configuration");
        static std::unique_ptr<ClientConfig> client = std::make_unique<ClientConfig>(client_);
        app_->callback([this](){
            this->execute();
        });
    }
    void NetworkConfig::execute(){
    }

    Configuration::Configuration(CLI::App* app):
    app_(app){
        app_->require_subcommand(1);
        CLI::App* user_=app_->add_subcommand(
                "user","user configuration");
        static std::unique_ptr<parse::UserConfig> user_obj = std::make_unique<parse::UserConfig>(user_);
        CLI::App* system_=app_->add_subcommand(
                "system","system configuration");
        static std::unique_ptr<parse::SystemConfig> system_obj = std::make_unique<parse::SystemConfig>(system_);
        CLI::App* network_=app_->add_subcommand(
                "network","network configuration");
        static std::unique_ptr<parse::NetworkConfig> network_obj = std::make_unique<parse::NetworkConfig>(network_);
        app_->callback([this](){
            this->execute();
        });
    }
    void Configuration::execute(){
    }
}