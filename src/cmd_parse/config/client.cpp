#include "cmd_parse/config/client.h"

namespace parse{
    ClientConfig::OptionsSetting::OptionsSetting(CLI::App* app):
    app_(app){
        reuse_addr_=app->add_flag("--reuse_address")
        ->default_val(false)
        ->capture_default_str();
        reuse_port_=app->add_flag("--reuse_port")
        ->default_val(false)
        ->capture_default_str();
        broadcast_socket_=app->add_flag("--broadcast")
        ->default_val(false)
        ->capture_default_str();
        dont_route_=app->add_flag("--dont-route")
        ->default_val(false)
        ->capture_default_str();
        keep_alive_=app->add_flag("--keep-alive")
        ->default_val(false)
        ->capture_default_str();
        linger_=app->add_option("--linger");
        timeout_send_=app->add_option("--send-timeout")
        ->default_val(timeval{.tv_sec=0,.tv_usec=0})
        ->capture_default_str();
        timeout_recv_=app->add_option("--recv-timeout")
        ->default_val(timeval{.tv_sec=0,.tv_usec=0})
        ->capture_default_str();
        bufsiz_send_=app->add_option("--send-bufsiz")
        ->default_val(default_bufsiz_send)
        ->capture_default_str();
        bufsiz_recv_=app->add_option("--recv-bufsiz")
        ->default_val(default_bufsiz_recv)
        ->capture_default_str();
    }
    void ClientConfig::OptionsSetting::execute(network::ConnectionOptions& options){
        if(reuse_addr_->count())
            options.reuse_address_={true,{}};
        if(reuse_port_->count())
            options.reuse_port_={true,{}};
        if(broadcast_socket_->count())
            options.broadcast_socket_={true,{}};
        if(dont_route_->count())
            options.dont_route_={true,{}};
        if(keep_alive_->count())
            options.keep_alive_={true,{}};
        if(linger_->count())
            options.linger_={linger_->as<linger>(),{}};
        if(timeout_send_->count())
            options.timeout_send_={timeout_send_->as<timeval>(),{}};
        if(timeout_recv_->count())
            options.timeout_input_={timeout_recv_->as<timeval>(),{}};
        if(bufsiz_send_->count())
            options.buffer_size_out_={bufsiz_send_->as<int>(),{}};
        if(bufsiz_recv_->count())
            options.buffer_size_in_={bufsiz_recv_->as<int>(),{}};
    }
    ClientConfig::Add::Add(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        bind_address_=app_
        ->add_option("--bind-addr",
        "specified binding address of client")
        ->default_val(std::nullopt) //automatic choice of network device by OS
        ->capture_default_str();
        protocol_=app_
        ->add_option("--proto",
        "protocol of deploying client")
        ->default_val(std::string(network::protocol::to_text(network::Protocol::TCP)))
        ->capture_default_str();
        process_timeout_=app_
        ->add_option("--timeout-processes",
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        parallel_=app_
        ->add_option("--jobs",
        "using CPU cores by deploying client")
        ->default_val(std::thread::hardware_concurrency())
        ->capture_default_str();
        events_handled_=app_
        ->add_option("--events-handle",
        "maximum number of processed pended events")
        ->default_val(50)
        ->capture_default_str();
        options_=app_
        ->add_subcommand("opt",
        "client options setting");
    }

    void ClientConfig::Add::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" already exists");
        ::network::client::Settings settings_;
        if(bind_address_->count()){
            settings_.binded_addr_=
            bind_address_->as<std::optional<network::Address>>();
        }
        if(protocol_->count()){
            settings_.protocol_=
            protocol_->as<network::Protocol>();
        }
        if(process_timeout_->count())
            settings_.timeout_seconds_processes_=
            process_timeout_->as<int32_t>();
        if(parallel_->count()){
            if(uint32_t jobs = parallel_->as<uint32_t>();
                jobs>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=
                parallel_->as<uint32_t>();
        }
        if(events_handled_->count())
            settings_.number_events_=
                events_handled_->as<uint32_t>();
        if(options_->count()){
            static OptionsSetting opt_settings(options_);
            network::ConnectionOptions connopt;
            opt_settings.execute(connopt);
        }
    }

    ClientConfig::Remove::Remove(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
    }

    void ClientConfig::Remove::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().client_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" don't exists");
        ::app().config().client_config().remove(name);
    }

    ClientConfig::Modify::Modify(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        bind_address_=app_
        ->add_option("--bind-addr",
        "specified binding address of client")
        ->default_val(std::nullopt) //automatic choice of network device by OS
        ->capture_default_str();
        protocol_=app_
        ->add_option("--proto",
        "protocol of deploying client")
        ->default_val(std::string(network::protocol::to_text(network::Protocol::TCP)))
        ->capture_default_str();
        process_timeout_=app_
        ->add_option("--timeout-processes",
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        parallel_=app_
        ->add_option("--jobs",
        "using CPU cores by deploying client")
        ->default_val(std::thread::hardware_concurrency())
        ->capture_default_str();
        events_handled_=app_
        ->add_option("--events-handle",
        "maximum number of processed pended events")
        ->default_val(50)
        ->capture_default_str();
        options_=app_
        ->add_subcommand("opt",
        "client options setting");
    }

    void ClientConfig::Modify::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" already exists");
        ::network::client::Settings& settings_=*::app().config().
            client_config().get_config(name);
        if(bind_address_->count()){
            settings_.binded_addr_=
            bind_address_->as<std::optional<network::Address>>();
        }
        if(protocol_->count()){
            settings_.protocol_=
            protocol_->as<network::Protocol>();
        }
        if(process_timeout_->count())
            settings_.timeout_seconds_processes_=
            process_timeout_->as<int32_t>();
        if(parallel_->count()){
            if(uint32_t jobs = parallel_->as<uint32_t>();
                jobs>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=
                parallel_->as<uint32_t>();
        }
        if(events_handled_->count())
            settings_.number_events_=
                events_handled_->as<uint32_t>();
        if(options_->count()){
            static OptionsSetting opt_settings(options_);
            opt_settings.execute(settings_.options_);
        }
    }

    ClientConfig::Load::Load(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        path_=app_
        ->add_option("--path",
        "path of the loading file")
        ->required()->default_str("<PATH>");
    }

    void ClientConfig::Load::execute(){
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name))
            ::app().config().client_config().add_from_file(name,path_->
                as<std::string>());
        else ::app().config().client_config().modify_from_file(name,path_->
                as<std::string>());
    }

    ClientConfig::Print::Print(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
    }

    void ClientConfig::Print::execute(){
        std::string name = name_->as<std::string>();
        if(!::app().config().client_config().contains(
            name))
            ::app().config().client_config().print(name,std::cout);
    }
    
    ClientConfig::ClientConfig(CLI::App* app):
    app_(app)
    {
        add_ = app_->add_subcommand("add","add new client configuration");
        remove_ = app_->add_subcommand("remove","remove existing client configuration");
        modify_ = app_->add_subcommand("modify","modify existing client configuration");
        load_ = app_->add_subcommand("load","load client configuration from JSON file");
        print_all_ = app_->add_subcommand("print-all","print all accessible client configurations");
        print_named_ = app_->add_subcommand("print","print named client configuration (if exists)");
        current_ = app_->add_subcommand(
            "current","print current active client configuration");
        black_list_->add_subcommand("black-list","black list of hosts");
        white_list_->add_subcommand("white-list","white list of hosts");

    }
    void ClientConfig::execute(){
        if(app_->got_subcommand("add")){
            static Add add(add_);
            add.execute();
        }
        if(app_->got_subcommand("remove")){
            static Remove remove(remove_);
            remove.execute();
        }
        if(app_->got_subcommand("modify")){
            static Modify modify(modify);
            modify.execute();
        }
        if(app_->got_subcommand("load")){
            static Load load(load_);
            load.execute();
        }
        if(app_->got_subcommand("print-all"))
            ::app().config().client_config().print_all(std::cout);
        if(app_->got_subcommand("print")){
            static Print print(print_all_);
            print.execute();
        }
        if(app_->got_subcommand("current"))
            ::app().config().client_config().print_current(std::cout);
    }
}