#include "cmd_parse/config/server.h"
#include "cmd_parse/types_parse/address.h"
#include "types_parse/linger.h"
#include "types_parse/timeval.h"

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
        CLI::Option* linger_=app->add_option("--linger",options_.linger_.first)->
        default_val(default_val.linger_.first)->
        capture_default_str();
        CLI::Option* timeout_send_=app->add_option("--send-timeout",options_.timeout_send_.first)
        ->default_val(default_val.timeout_send_.first)
        ->capture_default_str();
        CLI::Option* timeout_recv_=app->add_option("--recv-timeout",options_.timeout_input_.first)
        ->default_val(default_val.timeout_input_.first)
        ->capture_default_str();
        CLI::Option* bufsiz_send_=app->add_option("--send-bufsiz",options_.buffer_size_out_.first)
        ->default_val(default_val.buffer_size_out_.first)
        ->capture_default_str();
        CLI::Option* bufsiz_recv_=app->add_option("--recv-bufsiz",options_.buffer_size_in_.first)
        ->default_val(default_val.buffer_size_in_.first)
        ->capture_default_str();
        app_->callback([this](){execute();});
    }
    void ServerConfig::OptionsSetting::execute(){}
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
        ->default_val(network::Protocol::TCP)
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
        opt_settings = std::make_unique<OptionsSetting>(options_);
        app_->callback([this](){execute();});
    }

    void ServerConfig::Add::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" already exists");
        ::network::server::Settings settings_;
        if(app_->count("--host")){
            settings_.host_=
            host_val_;
        }
        if(app_->count("--port")){
            settings_.port_=
            port_val_;
        }
        if(app_->count("--proto")){
            settings_.protocol_=
            protocol_val_;
        }
        if(app_->count("--timeout-processes"))
            settings_.timeout_seconds_processes_=
            process_timeout_val_;
        if(app_->count("--jobs")){
            if(jobs_val_>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=
                jobs_val_;
        }
        if(app_->count("--events-handle"))
            settings_.number_events_=
                events_handled_val_;
        if(app_->got_subcommand("opt")){
            opt_settings->execute();
            settings_.options_ = opt_settings->options();
        }
    }

    ServerConfig::Remove::Remove(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",
            name_val_,
            "network configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void ServerConfig::Remove::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().server_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" don't exists");
        ::app().config().server_config().remove(name_val_);
    }

    ServerConfig::Modify::Modify(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name","network configuration name")
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
        ->default_val(network::Protocol::TCP)
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
        opt_settings = std::make_unique<OptionsSetting>(options_);
        app_->callback([this](){execute();});
    }

    void ServerConfig::Modify::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" already exists");
        ::network::server::Settings& settings_=*::app().config().
            server_config().get_config(name_val_);
        if(app_->count("--host")){
            settings_.host_=
            host_val_;
        }
        if(app_->count("--port")){
            settings_.port_=
            port_val_;
        }
        if(app_->count("--proto")){
            settings_.protocol_=
            protocol_val_;
        }
        if(app_->count("--timeout-processes"))
            settings_.timeout_seconds_processes_=
            process_timeout_val_;
        if(app_->count("--jobs")){
            if(jobs_val_>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=
                jobs_val_;
        }
        if(app_->count("--events-handle"))
            settings_.number_events_=
                events_handled_val_;
        if(app_->got_subcommand("opt")){
            opt_settings->execute();
            settings_.options_ = opt_settings->options();
        }
    }

    ServerConfig::Load::Load(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name","network configuration name")
        ->required();
        CLI::Option* path_=app_
        ->add_option("--path",
        "path of the loading file")
        ->required()->default_str("<PATH>");
        app_->callback([this](){execute();});
    }

    void ServerConfig::Load::execute(){
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name_val_))
            ::app().config().server_config().add_from_file(name_val_,path_val_);
        else ::app().config().server_config().modify_from_file(name_val_,path_val_);
    }

    ServerConfig::Print::Print(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void ServerConfig::Print::execute(){
        if(!::app().config().server_config().contains(
            name_val_))
            ::app().config().server_config().print(name_val_,std::cout);
    }

    ServerConfig::BlackList::BlackList(CLI::App* app):
    app_(app){
        CLI::Option* add_ = app_->add_option("--add,-a", host_val_,"add new host to black list");
        CLI::Option* remove_ = app_->add_option("--remove,-r", host_val_, "remove existing host from black list");
        CLI::Option* print_ = app_->add_option("--print,-p", "print black list");
        app_->callback([this](){execute();});
    }

    void ServerConfig::BlackList::execute(){
        if(app_->count("--add"))
            ::app().config().server_config().push_to_black_list(host_val_);
        if(app_->count("--remove"))
            ::app().config().server_config().remove_from_black_list(host_val_);
        if(app_->count("--print"))
            ::app().config().server_config().print_black_list(std::cout);
    }

    ServerConfig::WhiteList::WhiteList(CLI::App* app):
    app_(app){
        CLI::Option* add_ = app_->add_option("--add,-a", host_val_, "add new host to white list");
        CLI::Option* remove_ = app_->add_option("--remove,-r", host_val_, "remove existing host from white list");
        CLI::Option* print_ = app_->add_option("--print,-p", "print white list");
        app_->callback([this](){execute();});
    }

    void ServerConfig::WhiteList::execute(){
        if(app_->count("--add"))
            ::app().config().server_config().push_to_white_list(host_val_);
        if(app_->count("--remove"))
            ::app().config().server_config().remove_from_white_list(host_val_);
        if(app_->count("--print"))
            ::app().config().server_config().print_white_list(std::cout);
    }
    
    ServerConfig::ServerConfig(CLI::App* app):
    app_(app)
    {
        app_->require_subcommand(1);
        CLI::App* add_ = app_->add_subcommand("add","add new server configuration");
        static std::unique_ptr<Add> add = std::make_unique<Add>(add_);
        CLI::App* remove_ = app_->add_subcommand("remove","remove existing server configuration");
        static std::unique_ptr<Remove> remove = std::make_unique<Remove>(remove_);
        CLI::App* modify_ = app_->add_subcommand("modify","modify existing server configuration");
        static std::unique_ptr<Modify> modify = std::make_unique<Modify>(modify_);
        CLI::App* load_ = app_->add_subcommand("load","load server configuration from JSON file");
        static std::unique_ptr<Load> load = std::make_unique<Load>(load_);
        CLI::App* print_all_ = app_->add_subcommand("print-all","print all accessible server configurations");
        CLI::App* print_named_ = app_->add_subcommand("print","print named server configuration (if exists)");
        static std::unique_ptr<Print> print = std::make_unique<Print>(print_named_);
        CLI::App* current_ = app_->add_subcommand(
            "current","print current active server configuration");
        CLI::App* black_list_ = app_->add_subcommand("black-list","black list of hosts");
        static std::unique_ptr<BlackList> black_list = std::make_unique<BlackList>(black_list_);
        CLI::App* white_list_ = app_->add_subcommand("white-list","white list of hosts");
        static std::unique_ptr<WhiteList> white_list = std::make_unique<WhiteList>(white_list_);
        app_->callback([this](){execute();});
    }
    void ServerConfig::execute(){
        if(app_->got_subcommand("print-all"))
            ::app().config().server_config().print_all(std::cout);
        if(app_->got_subcommand("current"))
            ::app().config().server_config().print_current(std::cout);
    }
}