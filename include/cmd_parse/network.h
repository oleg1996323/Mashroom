#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include <boost/program_options.hpp>
#include <set>
#include <CLI/CLI.hpp>

namespace parse{

    void init_network(CLI::App* app);


    class Network{
        Network(CLI::App* app){
            auto launch = app->add_subcommand("launch-server","");
            std::error_code err;
            if(*launch){
                ::Mashroom::instance().server().launch(err);
                if(err!=std::error_code())
                    std::cout<<err.message()<<std::endl;
            }
            auto close = app->add_subcommand("close","close all connections with launched server");
            {
                bool wait = false;
                close->add_flag("--wait",wait,"wait finishing all connection-process");
                uint16_t timeout = 0;
                close->add_option("--timeout",timeout,"timeout closing in seconds");
                if(*close)
                    ::Mashroom::instance().server().close(wait,timeout);
            }
            auto suspend = app->add_subcommand("suspend","suspend connections with launched server");
            {
                bool wait = false;
                uint16_t timeout = 0;
                suspend->add_flag("--wait", wait, "wait finishing all connection-process before suspending");
                suspend->add_option("--timeout", timeout, "timeout suspending in seconds");
                if(*suspend)
                    ::Mashroom::instance().server().collapse(wait,timeout);
            }
        }
        
    };
}