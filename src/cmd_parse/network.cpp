#include "cmd_parse/network.h"
#include "program/mashroom.h"
#include "web/server/connection_process.h"

namespace parse{

    Network::Network(CLI::App* app):app_(app){
        CLI::App* launch_ = app_->add_subcommand(
                "launch","");
        CLI::App* close_=app_->add_subcommand(
                "close","close all connections with launched server");
        CLI::App* suspend_=app_->add_subcommand(
                "suspend","suspend all connections with launched server");
        close_->add_option("--timeout",
                timeout_,"timeout closing in seconds")->
                default_val(0);
        suspend_->add_option("--timeout",
                timeout_, "timeout suspending in seconds")->
                default_val(0);
        app_->callback([this](){
            this->execute();
        });
    }
    void Network::execute(){
        if(app_->got_subcommand("launch")){
            std::error_code err;
            ::Mashroom::instance().server().configure(::app().config().server_config().current_settings(),err);
            if(err)
                throw std::runtime_error(err.message());
            ::Mashroom::instance().server().
                set_processes_at_connections<network::ServerConnectionProcess>();
            ::Mashroom::instance().server().launch(err);
            if(err)
                throw std::runtime_error(err.message());
        }
        if(app_->got_subcommand("close"))
            ::Mashroom::instance().server().close(timeout_);
        if(app_->got_subcommand("suspend"))
            ::Mashroom::instance().server().collapse(timeout_);
    }
}