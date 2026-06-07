#include "cmd_parse/config/user.h"


namespace parse{
    UserConfig::Add::Add(CLI::App* app){
        name_ = app_
        ->add_option("--name","user configuration name")
        ->required();
        output_files_root_dir_=app_
        ->add_option("--output-root-dir",
        "the root directory where user files will be placed")
        ->default_val((fs::path(getenv("HOME"))/
            "Mashroom"/"output").string())
        ->capture_default_str();
        index_upd_ti_=app_
        ->add_option("--index-update-ti",
        "index update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        mashroom_upd_ti_=app_
        ->add_option("--mashroom-update-ti",
        "Mashroom update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
    }

    void UserConfig::Add::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("user-configuration "s+
                name+" already exists");
        ::user::Settings settings_;
        if(output_files_root_dir_->count()){
            settings_.output_files_root_dir=
            output_files_root_dir_->as<std::string>();
        }
        if(index_upd_ti_->count()){
            settings_.index_update_ti=
            index_upd_ti_->as<DateTimeDiff>();
        }
        if(mashroom_upd_ti_->count()){
            settings_.mashroom_update_ti=
            mashroom_upd_ti_->as<DateTimeDiff>();
        }
        ::app().config().user_config().add(
            name,std::move(settings_));
    }
    UserConfig::Remove::Remove(CLI::App* app){
        name_ = app_
        ->add_option("--name","user configuration name")
        ->required();
    }

    void UserConfig::Remove::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("user-configuration "s+
                name+" don't exists");
        ::app().config().user_config().remove(name);
    }

    UserConfig::Modify::Modify(CLI::App* app){
        name_ = app_
        ->add_option("--name","user configuration name")
        ->required();
        output_files_root_dir_=app_
        ->add_option("--output-root-dir",
        "the root directory where user files will be placed")
        ->default_val((fs::path(getenv("HOME"))/
            "Mashroom"/"output").string())
        ->capture_default_str();
        index_upd_ti_=app_
        ->add_option("--index-update-ti",
        "index update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        mashroom_upd_ti_=app_
        ->add_option("--mashroom-update-ti",
        "Mashroom update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
    }

    void UserConfig::Modify::execute(){
        using namespace std::string_literals;
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name))
            throw std::runtime_error("user-configuration "s+
                name+" already exists");
        ::user::Settings& settings_=*::app().config().
            user_config().get_config(name);
        if(output_files_root_dir_->count()){
            settings_.output_files_root_dir=
            output_files_root_dir_->as<std::string>();
        }
        if(index_upd_ti_->count()){
            settings_.index_update_ti=
            index_upd_ti_->as<DateTimeDiff>();
        }
        if(mashroom_upd_ti_->count()){
            settings_.mashroom_update_ti=
            mashroom_upd_ti_->as<DateTimeDiff>();
        }
    }

    UserConfig::Load::Load(CLI::App* app){
        name_ = app_
        ->add_option("--name","user configuration name")
        ->required();
        path_=app_
        ->add_option("--path",
        "path of the loading file")
        ->required()->default_str("<PATH>");
    }

    void UserConfig::Load::execute(){
        std::string name = name_->as<std::string>();
        if(name=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name))
            ::app().config().user_config().add_from_file(name,path_->
                as<std::string>());
        else ::app().config().user_config().modify_from_file(name,path_->
                as<std::string>());
    }

    UserConfig::Print::Print(CLI::App* app){
        name_ = app_
        ->add_option("--name","user configuration name")
        ->required();
    }

    void UserConfig::Print::execute(){
        std::string name = name_->as<std::string>();
        if(!::app().config().user_config().contains(
            name))
            ::app().config().user_config().print(name,std::cout);
    }

    UserConfig::UserConfig(CLI::App* app):
    app_(app)
    {
        add_ = app_->add_subcommand("add","add new user configuration");
        remove_ = app_->add_subcommand("remove","remove existing user configuration");
        modify_ = app_->add_subcommand("modify","modify existing user configuration");
        load_ = app_->add_subcommand("load","load user configuration from JSON file");
        print_all_ = app_->add_subcommand("print-all","print all accessible user configurations");
        print_named_ = app_->add_subcommand("print","print named user configuration (if exists)");
        current_ = app_->add_subcommand(
            "current","print current active user configuration");
    }
    void UserConfig::execute(){
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
            ::app().config().user_config().print_all(std::cout);
        if(app_->got_subcommand("print")){
            static Print print(print_all_);
            print.execute();
        }
        if(app_->got_subcommand("current"))
            ::app().config().user_config().print_current(std::cout);
    }
}