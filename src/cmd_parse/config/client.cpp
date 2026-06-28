#include "cmd_parse/config/client.h"
#include "cmd_parse/types_parse/address.h"

namespace parse{
    ClientConfig::OptionsSetting::OptionsSetting(CLI::App* app):
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
    void ClientConfig::OptionsSetting::execute(){}
    ClientConfig::Add::Add(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        CLI::Option* bind_address_=app_
        ->add_option("--bind-addr",bind_address_val_,
        "specified binding address of client")
        ->default_val(std::nullopt) //automatic choice of network device by OS
        ->capture_default_str();
        CLI::Option* protocol_=app_
        ->add_option("--proto",protocol_val_,
        "protocol of deploying client")
        ->default_val(network::Protocol::TCP)
        ->capture_default_str();
        CLI::Option* process_timeout_=app_
        ->add_option("--timeout-processes",process_timeout_val_,
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        CLI::Option* parallel_=app_
        ->add_option("--jobs",jobs_val_,
        "using CPU cores by deploying client")
        ->default_val(std::thread::hardware_concurrency())
        ->capture_default_str();
        CLI::Option* events_handled_=app_
        ->add_option("--events-handle",events_handled_val_,
        "maximum number of processed pended events")
        ->default_val(50)
        ->capture_default_str();
        CLI::App* options_=app_
        ->add_subcommand("opt",
        "client options setting");
        app_->callback([this](){execute();});
    }

    void ClientConfig::Add::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" already exists");
        ::network::client::Settings settings_;
        if(app_->count("--bind-addr")){
            settings_.binded_addr_=bind_address_val_;
        }
        if(app_->count("--proto")){
            settings_.protocol_=protocol_val_;
        }
        if(app_->count("--timeout-processes"))
            settings_.timeout_seconds_processes_=process_timeout_val_;
        if(app_->count("jobs")){
            if(jobs_val_>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=jobs_val_;
        }
        if(app_->count("--events-handle"))
            settings_.number_events_=events_handled_val_;
        if(app_->count("opt")){
            opt_settings->execute();
            settings_.options_ = opt_settings->options();
        }
    }

    ClientConfig::Remove::Remove(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void ClientConfig::Remove::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().client_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" don't exists");
        ::app().config().client_config().remove(name_val_);
    }

    ClientConfig::Modify::Modify(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        CLI::Option* bind_address_=app_
        ->add_option("--bind-addr",bind_address_val_,
        "specified binding address of client")
        ->default_val(std::nullopt) //automatic choice of network device by OS
        ->capture_default_str();
        CLI::Option* protocol_=app_
        ->add_option("--proto",protocol_val_,
        "protocol of deploying client")
        ->default_val(network::Protocol::TCP)
        ->capture_default_str();
        CLI::Option* process_timeout_=app_
        ->add_option("--timeout-processes",
            process_timeout_val_,
        "timeout of interconnection")
        ->default_val(-1)
        ->capture_default_str();
        CLI::Option* jobs_=app_
        ->add_option("--jobs",jobs_val_,
        "using CPU cores by deploying client")
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
        "client options setting");
        app_->callback([this](){execute();});
    }

    void ClientConfig::Modify::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("network-configuration "s+
                name_val_+" already exists");
        ::network::client::Settings& settings_=*::app().config().
            client_config().get_config(name_val_);
        if(app_->count("--bind-addr")){
            settings_.binded_addr_=bind_address_val_;
        }
        if(app_->count("--proto")){
            settings_.protocol_=protocol_val_;
        }
        if(app_->count("--timeout-processes"))
            settings_.timeout_seconds_processes_=process_timeout_val_;
        if(app_->count("jobs")){
            if(jobs_val_>std::thread::hardware_concurrency())
                settings_.num_threads_pool_=
                    std::thread::hardware_concurrency();
            else
                settings_.num_threads_pool_=jobs_val_;
        }
        if(app_->count("--events-handle"))
            settings_.number_events_=events_handled_val_;
        if(app_->count("opt")){
            opt_settings->execute();
            settings_.options_ = opt_settings->options();
        }
    }

    ClientConfig::Load::Load(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"network configuration name")
        ->required();
        CLI::Option* path_=app_
        ->add_option("--path",
                path_val_,
                "path of the loading file")
        ->required()->default_str("<PATH>")->
        check(CLI::ExistingFile);
        app_->callback([this](){execute();});
    }

    void ClientConfig::Load::execute(){
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name_val_))
            ::app().config().client_config().add_from_file(name_val_,path_val_);
        else ::app().config().client_config().modify_from_file(name_val_,path_val_);
    }

    ClientConfig::Print::Print(CLI::App* app):
    app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",
                name_val_,
                "network configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void ClientConfig::Print::execute(){
        if(!::app().config().client_config().contains(
            name_val_))
            ::app().config().client_config().print(name_val_,std::cout);
    }
    
    ClientConfig::ClientConfig(CLI::App* app):
    app_(app)
    {
        app_->require_subcommand(1);
        CLI::App* add_ = app_->add_subcommand("add","add new client configuration");
        static std::unique_ptr<Add> add = std::make_unique<Add>(add_);
        CLI::App* remove_ = app_->add_subcommand("remove","remove existing client configuration");
        static std::unique_ptr<Remove> remove = std::make_unique<Remove>(remove_);
        CLI::App* modify_ = app_->add_subcommand("modify","modify existing client configuration");
        static std::unique_ptr<Modify> modify = std::make_unique<Modify>(modify_);
        CLI::App* load_ = app_->add_subcommand("load","load client configuration from JSON file");
        static std::unique_ptr<Load> load = std::make_unique<Load>(load_);
        CLI::App* print_all_ = app_->add_subcommand("print-all","print all accessible client configurations");
        CLI::App* print_named_ = app_->add_subcommand("print","print named client configuration (if exists)");
        static std::unique_ptr<Print> print = std::make_unique<Print>(print_named_);
        CLI::App* current_ = app_->add_subcommand(
            "current","print current active client configuration");
        app_->callback([this](){execute();});
    }
    void ClientConfig::execute(){
        if(app_->got_subcommand("print-all"))
            ::app().config().client_config().print_all(std::cout);
        if(app_->got_subcommand("current"))
            ::app().config().client_config().print_current(std::cout);
    }
}