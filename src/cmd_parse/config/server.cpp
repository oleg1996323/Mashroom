#include "cmd_parse/config/server.h"
#include "cmd_parse/types_parse/address.h"

namespace parse{
    ServerConfig::OptionsSetting::OptionsSetting(CLI::App* app):
    app_(app){
        network::ConnectionOptions default_val;
        CLI::Option* reuse_addr_=app->add_flag("--reuse_address")
        ->default_val(default_val.reuse_address_.first)
        ->capture_default_str();
        CLI::Option* reuse_port_=app->add_flag("--reuse_port")
        ->default_val(default_val.reuse_port_.first)
        ->capture_default_str();
        CLI::Option* broadcast_socket_=app->add_flag("--broadcast")
        ->default_val(default_val.broadcast_socket_.first)
        ->capture_default_str();
        CLI::Option* dont_route_=app->add_flag("--dont-route")
        ->default_val(default_val.dont_route_.first)
        ->capture_default_str();
        CLI::Option* keep_alive_=app->add_flag("--keep-alive")
        ->default_val(default_val.keep_alive_.first)
        ->capture_default_str();
        CLI::Option* linger_=app->add_option("--linger",options_.linger_)->
        default_val(default_val.linger_.first)->
        capture_default_str();
        CLI::Option* timeout_send_=app->add_option("--send-timeout",options_.timeout_send_)
        ->default_val(default_val.timeout_send_.first)
        ->capture_default_str();
        CLI::Option* timeout_recv_=app->add_option("--recv-timeout",options_.timeout_input_)
        ->default_val(default_val.timeout_input_.first)
        ->capture_default_str();
        CLI::Option* bufsiz_send_=app->add_option("--send-bufsiz",options_.buffer_size_out_)
        ->default_val(default_val.buffer_size_out_.first)
        ->capture_default_str();
        CLI::Option* bufsiz_recv_=app->add_option("--recv-bufsiz",options_.buffer_size_in_)
        ->default_val(default_val.buffer_size_in_.first)
        ->capture_default_str();
        app_->callback([this](){execute();});
    }
    void ServerConfig::OptionsSetting::execute(network::ConnectionOptions& options){}
    ServerConfig::Add::Add(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        CLI::Option* host_=app_
        ->add_option("--host,-H",
            host_val_,
        "host of deploying server")
        ->default_val("0.0.0.0") //automatic choice of network device by OS
        ->capture_default_str();
        CLI::Option* port_=app_
        ->add_option("--port,-P", //automatic choice of port by OS
            port_val_,
        "port of deploying server")
        ->default_val(0)
        ->capture_default_str();
        CLI::Option* protocol_=app_
        ->add_option("--proto",
            protocol_val_,
        "protocol of deploying server")
        ->default_val(std::string(network::protocol::to_text(network::Protocol::TCP)))
        ->capture_default_str();
        CLI::Option* process_timeout_=app_
        ->add_option("--timeout-processes",
            process_timeout_val_,
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        CLI::Option* parallel_=app_
        ->add_option("--jobs",
            jobs_val_,
        "using CPU cores by deploying server")
        ->default_val(std::thread::hardware_concurrency())
        ->capture_default_str();
        CLI::Option* events_handled_=app_
        ->add_option("--events-handle",
            events_handled_val_,
        "maximum number of processed pended events")
        ->default_val(50)
        ->capture_default_str();
        CLI::App* options_=app_
        ->add_subcommand("opt",
        "server options setting");
    }

    void ServerConfig::Add::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" already exists");
        ::network::server::Settings settings_;
        if(host_->count()){
            settings_.host_=
            host_->as<std::string>();
        }
        if(port_->count()){
            settings_.port_=
            port_->as<int32_t>();
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

    ServerConfig::Remove::Remove(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
    }

    void ServerConfig::Remove::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().server_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" don't exists");
        ::app().config().server_config().remove(name);
    }

    ServerConfig::Modify::Modify(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        host_=app_
        ->add_option("--host,-H",
        "host of deploying server")
        ->default_val("0.0.0.0") //automatic choice of network device by OS
        ->capture_default_str();
        port_=app_
        ->add_option("--port,-P", //automatic choice of port by OS
        "port of deploying server")
        ->default_val(0)
        ->capture_default_str();
        protocol_=app_
        ->add_option("--proto",
        "protocol of deploying server")
        ->default_val(std::string(network::protocol::to_text(network::Protocol::TCP)))
        ->capture_default_str();
        process_timeout_=app_
        ->add_option("--timeout-processes",
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        parallel_=app_
        ->add_option("--jobs",
        "using CPU cores by deploying server")
        ->default_val(std::thread::hardware_concurrency())
        ->capture_default_str();
        events_handled_=app_
        ->add_option("--events-handle",
        "maximum number of processed pended events")
        ->default_val(50)
        ->capture_default_str();
        options_=app_
        ->add_subcommand("opt",
        "server options setting");
    }

    void ServerConfig::Modify::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("network-configuration "s+
                name+" already exists");
        ::network::server::Settings& settings_=*::app().config().
            server_config().get_config(name);
        if(host_->count()){
            settings_.host_=
            host_->as<std::string>();
        }
        if(port_->count()){
            settings_.port_=
            port_->as<int32_t>();
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

    ServerConfig::Load::Load(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        path_=app_
        ->add_option("--path",
        "path of the loading file")
        ->required()->default_str("<PATH>");
    }

    void ServerConfig::Load::execute(){
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name))
            ::app().config().server_config().add_from_file(name,path_->
                as<std::string>());
        else ::app().config().server_config().modify_from_file(name,path_->
                as<std::string>());
    }

    ServerConfig::Print::Print(CLI::App* app):
    app_(app){
        name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
    }

    void ServerConfig::Print::execute(){
        std::string name = name_->as<std::string>();
        if(!::app().config().server_config().contains(
            name))
            ::app().config().server_config().print(name,std::cout);
    }

    ServerConfig::BlackList::BlackList(CLI::App* app):
    app_(app){
        add_ = app_->add_option("--add,-a", "add new host to black list");
        remove_ = app_->add_option("--remove,-r", "remove existing host from black list");
        print_ = app_->add_option("--print,-p", "print black list");
    }

    void ServerConfig::BlackList::execute(){
        if(add_->count())
            ::app().config().server_config().push_to_black_list(add_->as<std::string>());
        if(remove_->count())
            ::app().config().server_config().remove_from_black_list(add_->as<std::string>());
        if(print_->count())
            ::app().config().server_config().print_black_list(std::cout);
    }

    ServerConfig::WhiteList::WhiteList(CLI::App* app):
    app_(app){
        add_ = app_->add_option("--add,-a", "add new host to white list");
        remove_ = app_->add_option("--remove,-r", "remove existing host from white list");
        print_ = app_->add_option("--print,-p", "print white list");
    }

    void ServerConfig::WhiteList::execute(){
        if(add_->count())
            ::app().config().server_config().push_to_white_list(add_->as<std::string>());
        if(remove_->count())
            ::app().config().server_config().remove_from_white_list(add_->as<std::string>());
        if(print_->count())
            ::app().config().server_config().print_white_list(std::cout);
    }
    
    ServerConfig::ServerConfig(CLI::App* app):
    app_(app)
    {
        add_ = app_->add_subcommand("add","add new server configuration");
        remove_ = app_->add_subcommand("remove","remove existing server configuration");
        modify_ = app_->add_subcommand("modify","modify existing server configuration");
        load_ = app_->add_subcommand("load","load server configuration from JSON file");
        print_all_ = app_->add_subcommand("print-all","print all accessible server configurations");
        print_named_ = app_->add_subcommand("print","print named server configuration (if exists)");
        current_ = app_->add_subcommand(
            "current","print current active server configuration");
        black_list_ = app_->add_subcommand("black-list","black list of hosts");
        white_list_ = app_->add_subcommand("white-list","white list of hosts");

    }
    void ServerConfig::execute(){
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
            ::app().config().server_config().print_all(std::cout);
        if(app_->got_subcommand("print")){
            static Print print(print_all_);
            print.execute();
        }
        if(app_->got_subcommand("current"))
            ::app().config().server_config().print_current(std::cout);
        if(app_->got_subcommand("black-list")){
            static BlackList black_list(black_list_);
            black_list.execute();
        }
        if(app_->got_subcommand("white-list")){
            static WhiteList white_list(white_list_);
            white_list.execute();
        }
    }
}