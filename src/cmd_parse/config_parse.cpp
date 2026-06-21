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
        log_dir_=app_
        ->add_option("--log-dir",
        "the directory where log files will be placed")
        ->default_val(::app().config().system_config().
            session_logging_directory())
        ->capture_default_str();
        config_dir_=app_
        ->add_option("--config-dir",
        "configurations directory")
        ->default_val(::app().config().system_config().
            configurations_directory().string())
        ->capture_default_str();
        cache_files_dir_=app_
        ->add_option("--cache-dir",
        "cache files directory")
        ->default_val(::app().config().system_config().
            cache_files_directory())
        ->capture_default_str();
        network_files_dir_=app_
        ->add_option("--network-dir",
        "network files directory")
        ->default_val(::app().config().system_config().
            network_files_directory())
        ->capture_default_str();
        app_->callback([this](){
            this->execute();
        });
    }
    void SystemConfig::execute(){
        if(log_dir_->count()){
            ::app().config().system_config().
            session_logging_directory(log_dir_->as<std::string>());
        }
        if(config_dir_->count()){
            ::app().config().system_config().
            configurations_directory(config_dir_->as<std::string>());
        }
        if(cache_files_dir_->count()){
            ::app().config().system_config().
            cache_files_directory(cache_files_dir_->as<std::string>());
        }
        if(network_files_dir_->count()){
            ::app().config().system_config().
            network_files_directory(network_files_dir_->as<std::string>());
        }
    }

    NetworkConfig::NetworkConfig(CLI::App* app):
    app_(app)
    {
        server_ = app_->add_subcommand("server","server configuration");
        client_ = app_->add_subcommand("client","client configuration");
        app_->callback([this](){
            this->execute();
        });
    }
    void NetworkConfig::execute(){
        if(app_->got_subcommand("server")){
            static ServerConfig server(server_);
            server.execute();
        }
        if(app_->got_subcommand("client")){
            static ClientConfig client(client_);
            client.execute();
        }
    }

    Configuration::Configuration(CLI::App* app):
    app_(app){
        CLI::App* user=app_->add_subcommand(
                "user","user configuration");
        static parse::UserConfig user_obj(user);
        CLI::App* system=app_->add_subcommand(
                "system","system configuration");
        static parse::SystemConfig system_obj(system);
        CLI::App* network=app_->add_subcommand(
                "network","network configuration");
        static parse::NetworkConfig network_obj(network);
        app_->callback([this](){
            this->execute();
        });
    }
    void Configuration::execute(){
    }
}