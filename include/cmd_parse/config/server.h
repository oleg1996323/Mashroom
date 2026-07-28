#pragma once
#include "sys/config.h"
#include "sys/application.h"
#include <CLI/CLI.hpp>
#include "OsterLib/network/connection_options.h"

namespace parse{
    class ServerConfig{
        class OptionsSetting{
            static constexpr uint32_t default_bufsiz_send =1024*8;
            static constexpr uint32_t default_bufsiz_recv =1024*8;
            
            CLI::App* app_;
            network::ConnectionOptions options_;
            public:
            OptionsSetting(CLI::App* app);
            void execute();
            const network::ConnectionOptions& options() const noexcept{
                return options_;
            }
        };

        class Add{
            CLI::App* app_;

            std::string name_val_;
            std::string host_val_;
            uint32_t port_val_;
            network::Protocol protocol_val_;
            network::Timeout process_timeout_val_;
            uint32_t jobs_val_;
            uint32_t events_handled_val_;
            std::unique_ptr<OptionsSetting> opt_settings;
            public:
            Add(CLI::App* app);
            void execute();
        };

        class Remove{
            CLI::App* app_;
            std::string name_val_;
            public:
            Remove(CLI::App* app);
            void execute();
        };

        class Modify{
            CLI::App* app_;
            std::string name_val_;
            std::string host_val_;
            uint32_t port_val_;
            network::Protocol protocol_val_;
            network::Timeout process_timeout_val_;
            uint32_t jobs_val_;
            uint32_t events_handled_val_;
            std::unique_ptr<OptionsSetting> opt_settings;
            public:
            Modify(CLI::App* app);
            void execute();
        };

        class Load{
            CLI::App* app_;
            std::string name_val_;
            std::string path_val_;
            public:
            Load(CLI::App* app);
            void execute();
        };

        class Print{
            CLI::App* app_;
            std::string name_val_;
            public:
            Print(CLI::App* app);
            void execute();
        };

        class BlackList{
            CLI::App* app_;
            std::string host_val_;
            public:
            BlackList(CLI::App* app);
            void execute();
        };

        class WhiteList{
            CLI::App* app_;
            std::string host_val_;
            public:
            WhiteList(CLI::App* app);
            void execute();
        };
        
        CLI::App* app_;
        public:
        ServerConfig(CLI::App* app);
        void execute();
    };
}