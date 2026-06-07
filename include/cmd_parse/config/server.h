#pragma once
#include "sys/config.h"
#include "sys/application.h"
#include <CLI/CLI.hpp>
#include "network/connection_options.h"

namespace parse{
    class ServerConfig{
        class OptionsSetting{
            static constexpr uint32_t default_bufsiz_send =1024*8;
            static constexpr uint32_t default_bufsiz_recv =1024*8;
            
            CLI::App* app_;
            CLI::Option* reuse_addr_;
            CLI::Option* reuse_port_;
            CLI::Option* broadcast_socket_;
            CLI::Option* dont_route_;
            CLI::Option* keep_alive_;
            CLI::Option* linger_;
            CLI::Option* timeout_send_;
            CLI::Option* timeout_recv_;
            CLI::Option* bufsiz_recv_;
            CLI::Option* bufsiz_send_;
            public:
            OptionsSetting(CLI::App* app);
            void execute(::network::ConnectionOptions& options);
        };

        class Add{
            CLI::App* app_;

            CLI::Option* name_;
            CLI::Option* host_;
            CLI::Option* port_;
            CLI::Option* protocol_;
            CLI::Option* process_timeout_;
            CLI::Option* parallel_;
            CLI::Option* events_handled_;
            CLI::App* options_;
            public:
            Add(CLI::App* app);
            void execute();
        };

        class Remove{
            CLI::App* app_;
            CLI::Option* name_;
            public:
            Remove(CLI::App* app);
            void execute();
        };

        class Modify{
            CLI::App* app_;
            CLI::Option* name_;
            CLI::Option* host_;
            CLI::Option* port_;
            CLI::Option* protocol_;
            CLI::Option* process_timeout_;
            CLI::Option* parallel_;
            CLI::Option* events_handled_;
            CLI::App* options_;
            public:
            Modify(CLI::App* app);
            void execute();
        };

        class Load{
            CLI::App* app_;
            CLI::Option* name_;
            CLI::Option* path_;
            public:
            Load(CLI::App* app);
            void execute();
        };

        class Print{
            CLI::App* app_;
            CLI::Option* name_;
            public:
            Print(CLI::App* app);
            void execute();
        };

        class BlackList{
            CLI::App* app_;
            CLI::Option* add_;
            CLI::Option* remove_;
            CLI::Option* print_;
            public:
            BlackList(CLI::App* app);
            void execute();
        };

        class WhiteList{
            CLI::App* app_;
            CLI::Option* add_;
            CLI::Option* remove_;
            CLI::Option* print_;
            public:
            WhiteList(CLI::App* app);
            void execute();
        };
        
        CLI::App* app_;
        CLI::App* add_;
        CLI::App* modify_;
        CLI::App* load_;
        CLI::App* remove_;
        CLI::App* print_all_;
        CLI::App* print_named_;
        CLI::App* current_;
        CLI::App* black_list_;
        CLI::App* white_list_;
        public:
        ServerConfig(CLI::App* app);
        void execute();
    };
}